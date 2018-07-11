#include "code_gen.hpp"

#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

#include "CodeGenVisitor.h"
#include "dag/DAGOperators.h"
#include "dag/type/array.hpp"
#include "dag/type/atomic.hpp"
#include "dag/utils/apply_visitor.hpp"

using boost::algorithm::join;
using boost::format;

namespace code_gen {
namespace cpp {

std::string AtomicTypeNameToRuntimeTypename(const std::string &type_name) {
    static const std::unordered_map<std::string, std::string> type_map = {
            {"float", "Float"},        //
            {"double", "Double"},      //
            {"int", "Int32"},          //
            {"long", "Int64"},         //
            {"bool", "Bool"},          //
            {"std::string", "String"}  //
    };
    return type_map.at(type_name);
}

std::string ComputeStructToValue(std::ostream &output, Context *const context,
                                 const std::string &input_var_name,
                                 const dag::type::Tuple *const tuple_type) {
    struct StructToValueVisitor
        : public Visitor<StructToValueVisitor, const dag::type::FieldType,
                         boost::mpl::list<                //
                                 const dag::type::Array,  //
                                 const dag::type::Atomic  //
                                 >::type,
                         std::string> {
        StructToValueVisitor(std::ostream &output, Context *const context,
                             // NOLINTNEXTLINE modernize-pass-by-value
                             const std::string &input_var_name)
            : input_var_name_(input_var_name),
              output_(output),
              context_(context) {}

        std::string operator()(const dag::type::Atomic *const type) const {
            const auto temp_var_name = context_->GenerateSymbolName("val");
            output_ << format("std::unique_ptr<%2%> %1%(new %2%());"
                              "%1%->value = %3%;") %
                               temp_var_name %
                               AtomicTypeNameToRuntimeTypename(type->type) %
                               input_var_name_;
            return temp_var_name + ".release()";
        }

        std::string operator()(const dag::type::Array * /*type*/) const {
            const auto temp_var_name = context_->GenerateSymbolName("val");
            output_ << format("std::unique_ptr<Array> %2%(new Array());"
                              "%2%->data = %1%.data;"
                              "%2%->shape = {%1%.shape[0]};") %
                               input_var_name_ % temp_var_name;
            return temp_var_name + ".release()";
        }

    private:
        std::ostream &output_;
        Context *const context_;
        const std::string input_var_name_;
    };

    output << "VectorOfValues v;"
              "v.emplace_back(new Tuple);";
    for (size_t i = 0; i < tuple_type->field_types.size(); i++) {
        const auto &type = tuple_type->field_types[i];
        const auto field_var_name = input_var_name + ".v" + std::to_string(i);
        StructToValueVisitor visitor(output, context, field_var_name);
        output << format("v[0]->as<Tuple>()->fields.emplace_back(%1%);") %
                          visitor.Visit(type);
    }
    return "v";
}

std::string ComputeValueToStruct(const std::string &input_var_name,
                                 const dag::type::Tuple *tuple_type,
                                 Context *context) {
    struct ValueToStructVisitor
        : public Visitor<ValueToStructVisitor, const dag::type::FieldType,
                         boost::mpl::list<                //
                                 const dag::type::Array,  //
                                 const dag::type::Atomic  //
                                 >::type,
                         std::string> {
        ValueToStructVisitor(
                // NOLINTNEXTLINE modernize-pass-by-value
                const std::string &input_var_name, Context *const context)
            : input_var_name_(input_var_name), context_(context) {}

        std::string operator()(const dag::type::Array *const type) const {
            const auto inner_type = type->tuple_type;
            const auto inner_typedef =
                    context_->tuple_type_descs().at(inner_type).get();

            return (format("{reinterpret_cast<%2%*>(%1%->as<Array>()->data),"
                           " %1%->as<Array>()->shape[0]}") %
                    input_var_name_ % inner_typedef->name)
                    .str();
        }

        std::string operator()(const dag::type::Atomic *const type) const {
            return (format("%1%->as<%2%>()->value") % input_var_name_ %
                    AtomicTypeNameToRuntimeTypename(type->type))
                    .str();
        }

        std::string operator()(const dag::type::FieldType *const type) const {
            throw std::runtime_error("Unknown field type: " +
                                     type->to_string());
        }

        std::string input_var_name_;
        Context *const context_;
    };

    ValueToStructVisitor visitor("", context);
    std::vector<std::string> field_values;
    for (size_t i = 0; i < tuple_type->field_types.size(); i++) {
        auto const &type = tuple_type->field_types[i];
        visitor.input_var_name_ = input_var_name + "->as<Tuple>()->fields[" +
                                  std::to_string(i) + "]";
        field_values.emplace_back(visitor.Visit(type));
    }

    const auto tuple_typedef = context->tuple_type_descs().at(tuple_type).get();
    return (format("%1%{%2%}") % tuple_typedef->name % join(field_values, ", "))
            .str();
}

FunctionDef GenerateExecuteTuples(DAG *const dag, Context *const context) {
    std::stringstream plan_body;

    CodeGenVisitor visitor(dag, context, plan_body);
    dag::utils::ApplyInReverseTopologicalOrder(dag, visitor.functor());

    // Compute execute function parameters
    struct CollectInputsVisitor
        : public Visitor<CollectInputsVisitor, DAGOperator,
                         boost::mpl::list<DAGParameterLookup>> {
        void operator()(DAGParameterLookup *op) { inputs_.emplace_back(op); }
        std::vector<DAGParameterLookup *> inputs_;
    };
    CollectInputsVisitor collec_inputs_visitor;
    for (auto const op : dag->operators()) {
        collec_inputs_visitor.Visit(op);
    }

    std::vector<std::string> packed_input_args;
    for (auto const op : collec_inputs_visitor.inputs_) {
        const std::string param_num = std::to_string(op->parameter_num);
        const auto tuple_type = op->tuple->type;

        // Parameters for function signature of 'execute_tuples'
        const auto tuple_typedef =
                context->tuple_type_descs().at(tuple_type).get();
        packed_input_args.emplace_back(
                (format("%1% input_%2%") % tuple_typedef->name % param_num)
                        .str());
    }

    auto const func_name = context->GenerateSymbolName("execute_tuples", true);
    auto sink = visitor.operator_descs_[dag->sink->id];
    context->definitions() <<  //
            format("%1% %5%(%2%) { "
                   "    %3%\n"
                   "    /** collecting the result **/"
                   "    %4%.open();"
                   "    auto result = %4%.next().value();"
                   "    %4%.close();"
                   "    return result;"
                   "}") %
                    sink.return_type->name % join(packed_input_args, ", ") %
                    plan_body.str() % sink.var_name % func_name;

    return {func_name, sink.return_type};
}

std::string GenerateExecuteValues(DAG *const dag, Context *const context) {
    // Generate plan function on tuples
    auto execute_tuples = GenerateExecuteTuples(dag, context);

    // Includes needed for generate_values
    context->includes().emplace("\"../../../runtime/values/array.hpp\"");
    context->includes().emplace("\"../../../runtime/values/atomics.hpp\"");
    context->includes().emplace("\"../../../runtime/values/value.hpp\"");
    context->includes().emplace("\"../../../runtime/values/json_parsing.hpp\"");
    context->includes().emplace("\"../../../runtime/values/tuple.hpp\"");

    context->declarations() << "using namespace runtime::values;" << std::endl;

    // Compute execute function parameters
    struct CollectInputsVisitor
        : public Visitor<CollectInputsVisitor, DAGOperator,
                         boost::mpl::list<DAGParameterLookup>> {
        void operator()(DAGParameterLookup *op) { inputs_.emplace_back(op); }
        std::vector<DAGParameterLookup *> inputs_;
    };
    CollectInputsVisitor collect_inputs_visitor;
    for (auto const op : dag->operators()) {
        collect_inputs_visitor.Visit(op);
    }

    std::vector<std::string> pack_input_args;
    for (auto const op : collect_inputs_visitor.inputs_) {
        const std::string param_num = std::to_string(op->parameter_num);

        // Packing of parameters as tuples
        const auto tuple_type = op->tuple->type;
        pack_input_args.emplace_back(ComputeValueToStruct(
                "inputs[" + param_num + "]", tuple_type, context));
    }

    // Main executable file: plan function on runtime values
    auto const return_type = dag->sink->tuple->type;
    const auto func_name = context->GenerateSymbolName("execute_values", true);
    std::stringstream temp_statements;
    const auto return_val =
            ComputeStructToValue(temp_statements, context, "ret", return_type);
    context->definitions() <<  //
            format("VectorOfValues %3%(const VectorOfValues "
                   "&inputs)"
                   "{"
                   "    const auto ret = %4%(%1%);"
                   "    %2%"
                   "    return %5%;"
                   "}") %
                    join(pack_input_args, ", ") % temp_statements.str() %
                    func_name % execute_tuples.name % return_val;

    return func_name;
}

std::string GenerateLlvmFunctor(
        Context *const context, const std::string &func_name_prefix,
        const std::string &llvm_ir,
        const std::vector<const StructDef *> &input_types,
        const std::string &return_type) {
    // Generate symbol names
    const auto func_name =
            context->GenerateSymbolName(func_name_prefix + "_llvm");
    const auto class_name =
            context->GenerateSymbolName(func_name_prefix + "_functor");

    // Emit LLVM code
    StoreLlvmCode(context, llvm_ir, func_name);

    // Prepare functor defintion
    std::vector<std::string> input_args;
    std::vector<std::string> call_args;
    std::vector<std::string> call_types;
    for (auto const &input_type : input_types) {
        // Each input tuple is one argument of the functor
        const auto input_var_name = "t" + std::to_string(input_args.size());
        input_args.emplace_back(input_type->name + " " + input_var_name);

        // Each collection of all input tuples is one argument to the function
        // --> construct values for the call
        for (auto const &field : input_type->names) {
            // NOLINTNEXTLINE performance-inefficient-string-concatenation
            call_args.emplace_back(input_var_name + "." + field);
        }

        // --> construct types for the declaration
        call_types.insert(call_types.begin(), input_type->types.begin(),
                          input_type->types.end());
    }

    // Emit functor definition
    context->definitions() <<  //
            format("class %s {"
                   "public:"
                   "    auto operator()(%s) {"
                   "        return %s(%s);"
                   "    }"
                   "};") %
                    class_name % join(input_args, ",") % func_name %
                    join(call_args, ",");

    // Emit function declaration
    context->declarations() << format("extern \"C\" { %s %s(%s); }") %
                                       return_type % func_name %
                                       join(call_types, ",");

    return class_name;
}

void StoreLlvmCode(Context *const context, const std::string &llvm_ir,
                   const std::string &func_name) {
    std::string patched_ir = llvm_ir;

    // Remove 'local_unnamed_addr', which is not llvm-3.7 compatible:
    std::regex reg1("local_unnamed_addr #.? ");
    patched_ir = std::regex_replace(patched_ir, reg1, "");

    // Replace the func name with ours
    std::regex reg("@cfuncnotuniquename");
    patched_ir = std::regex_replace(patched_ir, reg, "@\"" + func_name + "\"");

    // Write code
    context->llvm_code() << patched_ir;
}

const StructDef *EmitStructDefinition(Context *const context,
                                      const dag::type::Type *key,
                                      const std::vector<std::string> &types,
                                      const std::vector<std::string> &names) {
    if (context->tuple_type_descs().count(key) > 0) {
        return context->tuple_type_descs().at(key).get();
    }

    auto tuple_typedef = std::make_unique<StructDef>(
            context->GenerateSymbolName("tuple"), types, names);
    context->declarations() << tuple_typedef->ComputeDefinition();

    const auto ret =
            context->tuple_type_descs().emplace(key, std::move(tuple_typedef));
    assert(ret.second);

    return ret.first->second.get();
}

const StructDef *EmitTupleStructDefinition(Context *const context,
                                           const dag::type::Tuple *tuple) {
    class FieldTypeVisitor
        : public Visitor<FieldTypeVisitor, const dag::type::FieldType,
                         boost::mpl::list<const dag::type::Atomic,
                                          const dag::type::Array>::type,
                         std::string> {
    public:
        explicit FieldTypeVisitor(Context *const context) : context_(context) {}

        std::string operator()(const dag::type::Atomic *const field) {
            return field->type;
        }

        std::string operator()(const dag::type::Array *const field) {
            auto item_desc =
                    EmitTupleStructDefinition(context_, field->tuple_type);
            std::vector<std::string> names;
            std::vector<std::string> types;
            names.emplace_back("data");
            names.emplace_back(
                    boost::str(format("shape [%s]") % field->number_dim));
            types.emplace_back(boost::str(format("%s*") % item_desc->name));
            types.emplace_back("size_t");
            return EmitStructDefinition(context_, field, types, names)->name;
        }

        std::string operator()(const dag::type::FieldType *const field) const {
            throw std::runtime_error(
                    "Code gen encountered unknown field type: " +
                    field->to_string());
        }

    private:
        Context *const context_;
    };

    FieldTypeVisitor visitor(context);

    std::vector<std::string> types;
    std::vector<std::string> names;
    for (auto pos = 0; pos < tuple->field_types.size(); pos++) {
        auto f = tuple->field_types[pos];
        auto item_type = visitor.Visit(f);
        types.emplace_back(item_type);
        names.emplace_back("v" + std::to_string(pos));
    }
    return EmitStructDefinition(context, tuple, types, names);
}

}  // namespace cpp
}  // namespace code_gen
