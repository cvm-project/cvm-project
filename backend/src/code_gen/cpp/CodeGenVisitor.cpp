#include "CodeGenVisitor.h"

#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/mpl/list.hpp>

#include "dag/DAGOperators.h"
#include "dag/collection/array.hpp"
#include "dag/collection/atomic.hpp"
#include "utils/utils.h"
#include "utils/visitor.hpp"

using boost::algorithm::join;
using boost::format;

namespace code_gen {
namespace cpp {

void CodeGenVisitor::operator()(DAGCollection *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "CollectionSourceOperator");

    emitOperatorMake(var_name, "CollectionSourceOperator", op,
                     {op->add_index ? "true" : "false"}, {});
}

void CodeGenVisitor::operator()(DAGConstantTuple *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ConstantTupleOperator");

    const auto return_type = operator_descs_[op->id].return_type;
    const auto tuple_arg =
            (format("%s{%s}") % return_type->name % join(op->values, ","))
                    .str();

    emitOperatorMake(var_name, "ConstantTupleOperator", op, {}, {tuple_arg});
}

void CodeGenVisitor::operator()(DAGMap *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MapOperator");

    auto input_type = operator_descs_[dag_->predecessor(op)->id].return_type;
    auto return_type = operator_descs_[op->id].return_type;
    const std::string functor =
            visitLLVMFunc(*op, {input_type}, return_type->name);

    emitOperatorMake(var_name, "MapOperator", op, {}, {functor});
}

void CodeGenVisitor::operator()(DAGMaterializeRowVector *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MaterializeRowVectorOperator");

    auto input_type = operator_descs_[dag_->predecessor(op)->id].return_type;
    emitOperatorMake(var_name, "MaterializeRowVectorOperator", op,
                     {input_type->name}, {});
}

void CodeGenVisitor::operator()(DAGParameterLookup *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ConstantTupleOperator");

    const auto input_arg = "input_" + std::to_string(op->parameter_num);

    emitOperatorMake(var_name, "ConstantTupleOperator", op, {}, {input_arg});
}

void CodeGenVisitor::operator()(DAGReduce *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ReduceOperator");

    auto return_type = operator_descs_[op->id].return_type;
    const std::string functor =
            visitLLVMFunc(*op, {return_type, return_type}, return_type->name);

    emitOperatorMake(var_name, "ReduceOperator", op, {}, {functor});
}

void CodeGenVisitor::operator()(DAGRange *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "RangeSourceOperator");

    emitOperatorMake(var_name, "RangeSourceOperator", op, {}, {});
};

void CodeGenVisitor::operator()(DAGFilter *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "FilterOperator");

    auto input_type = operator_descs_[dag_->predecessor(op)->id].return_type;
    const std::string functor = visitLLVMFunc(*op, {input_type}, "bool");

    emitOperatorMake(var_name, "FilterOperator", op, {}, {functor});
};

void CodeGenVisitor::operator()(DAGJoin *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "JoinOperator");

    // Build key and value types
    // TODO(sabir): This currently only works for keys of size 1
    const auto up1Type = dag_->predecessor(op, 0)->tuple->type;
    const auto up2Type = dag_->predecessor(op, 1)->tuple->type;
    auto key_Tuple = up1Type->ComputeHeadTuple();

    auto key_type = EmitTupleStructDefinition(key_Tuple);

    auto value_tuple1 = up1Type->ComputeTailTuple();
    auto value_type1 = EmitTupleStructDefinition(value_tuple1);

    auto value_type2 = EmitTupleStructDefinition(up2Type);

    // Build operator
    std::vector<std::string> template_args = {key_type->name, value_type1->name,
                                              value_type2->name};

    emitOperatorMake(var_name, "JoinOperator", op, template_args);
};

void CodeGenVisitor::operator()(DAGCartesian *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "CartesianOperator");
    std::string pred2Tuple =
            operator_descs_[dag_->predecessor(op, 1)->id].return_type->name;
    std::vector<std::string> template_args = {pred2Tuple};
    emitOperatorMake(var_name, "CartesianOperator", op, template_args);
};

void CodeGenVisitor::visit_reduce_by_key(DAGOperator *op,
                                         const std::string &operator_name) {
    assert(dynamic_cast<DAGReduceByKey *>(op) != nullptr ||
           dynamic_cast<DAGReduceByKeyGrouped *>(op) != nullptr);

    const std::string var_name =
            CodeGenVisitor::visit_common(op, operator_name);

    // Build key and value types

    // TODO(sabir): This currently only works for keys of size 1
    auto key_type_tuple = op->tuple->type->ComputeHeadTuple();
    auto key_type = EmitTupleStructDefinition(key_type_tuple);

    auto value_type_tuple = op->tuple->type->ComputeTailTuple();
    auto value_type = EmitTupleStructDefinition(value_type_tuple);

    // Construct functor
    const std::string functor =
            visitLLVMFunc(*op, {value_type, value_type}, value_type->name);

    // Collect template arguments
    std::vector<std::string> template_args = {key_type->name, value_type->name};

    // Generate call
    emitOperatorMake(var_name, operator_name, op, template_args, {functor});
}

void CodeGenVisitor::operator()(DAGReduceByKey *op) {
    visit_reduce_by_key(op, "ReduceByKeyOperator");
}

void CodeGenVisitor::operator()(DAGReduceByKeyGrouped *op) {
    visit_reduce_by_key(op, "ReduceByKeyGroupedOperator");
}

void CodeGenVisitor::operator()(DAGOperator *const op) {
    throw std::runtime_error("CodeGen encountered unknown operator type: " +
                             op->name());
}

// TODO(ingo): This could be an independent visitor
std::string CodeGenVisitor::visit_common(DAGOperator *op,
                                         const std::string &operator_name) {
    plan_body_ << format("\n/** %s **/\n") % operator_name;
    const std::string var_name =
            context_->GenerateSymbolName("op_" + std::to_string(op->id), true);

    auto output_type = EmitTupleStructDefinition(op->tuple->type);

    operator_descs_.emplace(op->id,
                            OperatorDesc{op->id, var_name, output_type});
    context_->includes().insert("\"" + operator_name + ".h\"");
    return var_name;
}

std::string CodeGenVisitor::visitLLVMFunc(
        const DAGOperator &op,
        const std::vector<const StructDef *> &input_types,
        const std::string &return_type) {
    const std::string func_name =
            context_->GenerateSymbolName(op.name() + "_llvm_function");
    const std::string functor_name = snake_to_camel_string(func_name);
    storeLLVMCode(op.llvm_ir, func_name);
    emitLLVMFunctionWrapper(func_name, input_types, return_type);
    return functor_name + "()";
}

void CodeGenVisitor::emitOperatorMake(
        const std::string &variable_name, const std::string &operator_name,
        const DAGOperator *const op,
        const std::vector<std::string> &extra_template_args,
        const std::vector<std::string> &extra_args) {
    // First template argument: current tuple
    auto return_type = operator_descs_[op->id].return_type;
    std::vector<std::string> template_args = {return_type->name};

    // Take over extra template arguments
    template_args.insert(template_args.end(), extra_template_args.begin(),
                         extra_template_args.end());

    // Default input arguments: references to predecessors
    std::vector<std::string> args;
    for (size_t i = 0; i < op->num_in_ports(); i++) {
        const auto pred = dag_->predecessor(op, i);
        const auto arg = "&" + operator_descs_[pred->id].var_name;
        args.emplace_back(arg);
    }

    // Take over extra arguments
    args.insert(args.end(), extra_args.begin(), extra_args.end());

    // Generate call
    plan_body_ << format("auto %s = make%s<%s>(%s);") % variable_name %
                          operator_name % join(template_args, ",") %
                          join(args, ",");
}

void CodeGenVisitor::emitLLVMFunctionWrapper(
        const std::string &func_name,
        const std::vector<const StructDef *> &input_types,
        const std::string &return_type) {
    const std::string class_name = snake_to_camel_string(func_name);

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
    plan_body_ << format("class %s {"
                         "public:"
                         "    auto operator()(%s) {"
                         "        return %s(%s);"
                         "    }"
                         "};") %
                          class_name % join(input_args, ",") % func_name %
                          join(call_args, ",");

    // Emit function declaration
    context_->declarations() << format("extern \"C\" { %s %s(%s); }") %
                                        return_type % func_name %
                                        join(call_types, ",");
}

void CodeGenVisitor::storeLLVMCode(const std::string &ir,
                                   const std::string &func_name) {
    std::string patched_ir = ir;
    // the local_unnamed_addr std::string is not llvm-3.7 compatible:
    std::regex reg1("local_unnamed_addr #.? ");
    patched_ir = std::regex_replace(patched_ir, reg1, "");
    //        patched_ir = string_replace(patched_ir, "local_unnamed_addr #1
    //        ", "");
    // replace the func name with our
    std::regex reg("@cfuncnotuniquename");
    patched_ir = std::regex_replace(patched_ir, reg, "@\"" + func_name + "\"");
    // write code to the gen dir
    context_->llvm_code() << patched_ir;
}

const StructDef *CodeGenVisitor::EmitStructDefinition(
        const dag::type::Type *key, const std::vector<std::string> &types,
        const std::vector<std::string> &names) {
    if (context_->tuple_type_descs().count(key) > 0) {
        return context_->tuple_type_descs().at(key).get();
    }

    auto tuple_typedef = std::make_unique<StructDef>(
            context_->GenerateSymbolName("tuple"), types, names);
    context_->declarations() << tuple_typedef->ComputeDefinition();

    const auto ret =
            context_->tuple_type_descs().emplace(key, std::move(tuple_typedef));
    assert(ret.second);

    return ret.first->second.get();
}

const StructDef *CodeGenVisitor::EmitTupleStructDefinition(
        const dag::type::Tuple *tuple) {
    class FieldTypeVisitor
        : public Visitor<FieldTypeVisitor, const dag::type::FieldType,
                         boost::mpl::list<const dag::type::Atomic,
                                          const dag::type::Array>::type,
                         std::string> {
    public:
        explicit FieldTypeVisitor(CodeGenVisitor *pVisitor)
            : code_gen_visitor_(pVisitor) {}

        std::string operator()(const dag::type::Atomic *const field) {
            return field->type;
        }

        std::string operator()(const dag::type::Array *const field) {
            auto item_desc = code_gen_visitor_->EmitTupleStructDefinition(
                    field->tuple_type);
            std::vector<std::string> names;
            std::vector<std::string> types;
            names.emplace_back("data");
            names.emplace_back(
                    boost::str(format("shape [%s]") % field->number_dim));
            types.emplace_back(boost::str(format("%s*") % item_desc->name));
            types.emplace_back("size_t");
            return code_gen_visitor_->EmitStructDefinition(field, types, names)
                    ->name;
        }

        std::string operator()(const dag::type::FieldType *const field) const {
            throw std::runtime_error(
                    "Code gen encountered unknown field type: " +
                    field->to_string());
        }

    private:
        CodeGenVisitor *code_gen_visitor_;
    };

    FieldTypeVisitor visitor(this);

    std::vector<std::string> types;
    std::vector<std::string> names;
    for (auto pos = 0; pos < tuple->field_types.size(); pos++) {
        auto f = tuple->field_types[pos];
        auto item_type = visitor.Visit(f);
        types.emplace_back(item_type);
        names.emplace_back("v" + std::to_string(pos));
    }
    return EmitStructDefinition(tuple, types, names);
}
}  // namespace cpp
}  // namespace code_gen
