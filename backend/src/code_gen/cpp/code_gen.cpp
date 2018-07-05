#include "code_gen.hpp"

#include <ostream>
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

std::string ComputeStructToValue(const std::string &input_var_name,
                                 const std::string &output_var_name) {
    return (format("std::unique_ptr<Tuple> %2%(new Tuple());"
                   "%2%->fields.emplace_back(new Array);"
                   "%2%->fields[0]->as<Array>()->data = %1%.v0.data;"
                   "%2%->fields[0]->as<Array>()->shape = {%1%.v0.shape[0]};") %
            input_var_name % output_var_name)
            .str();
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
                   "    auto result = %4%.next().value;"
                   "    %4%.close();"
                   "    return result;"
                   "}") %
                    sink.return_type->name % join(packed_input_args, ", ") %
                    plan_body.str() % sink.var_name % func_name;

    return {func_name, sink.return_type};
}

std::string GenerateExecuteValues(DAG *const dag, Context *context) {
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
    const auto func_name = context->GenerateSymbolName("execute_values", true);
    context->definitions() <<  //
            format("VectorOfValues %3%(const VectorOfValues "
                   "&inputs)"
                   "{"
                   "    const auto ret = %4%(%1%);"
                   "    %2%"
                   "    VectorOfValues v;"
                   "    v.emplace_back(val.release());"
                   "    return v;"
                   "}") %
                    join(pack_input_args, ", ") %
                    ComputeStructToValue("ret", "val") % func_name %
                    execute_tuples.name;

    return func_name;
}

}  // namespace cpp
}  // namespace code_gen
