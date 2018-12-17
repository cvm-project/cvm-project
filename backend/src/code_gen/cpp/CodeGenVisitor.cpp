#include "CodeGenVisitor.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/mpl/list.hpp>

#include "code_gen.hpp"
#include "dag/DAGOperators.h"
#include "dag/collection/array.hpp"
#include "dag/collection/atomic.hpp"
#include "dag/type/array.hpp"
#include "utils/utils.h"
#include "utils/visitor.hpp"

using boost::algorithm::join;
using boost::format;

namespace code_gen {
namespace cpp {

void CodeGenVisitor::operator()(DAGAssertCorrectOpenNextClose *op) {
    const std::string var_name = CodeGenVisitor::visit_common(
            op, "AssertCorrectOpenNextCloseOperator");

    emitOperatorMake(var_name, "AssertCorrectOpenNextCloseOperator", op, {},
                     {});
}

void CodeGenVisitor::operator()(DAGRowScan *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "RowScanOperator");

    emitOperatorMake(var_name, "RowScanOperator", op,
                     {op->add_index ? "true" : "false"}, {});
}

void CodeGenVisitor::operator()(DAGConstantTuple *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ConstantTupleOperator");

    const auto return_type = operator_descs_[op].return_type;
    const auto tuple_arg =
            (format("%s{%s}") % return_type->name % join(op->values, ","))
                    .str();

    emitOperatorMake(var_name, "ConstantTupleOperator", op, {}, {tuple_arg});
}

void CodeGenVisitor::operator()(DAGColumnScan *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ColumnScanOperator");

    emitOperatorMake(var_name, "ColumnScanOperator", op,
                     {op->add_index ? "true" : "false"}, {});
}

void CodeGenVisitor::operator()(DAGGroupBy *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "GroupByOperator");

    emitOperatorMake(var_name, "GroupByOperator", op, {}, {});
}

void CodeGenVisitor::operator()(DAGMap *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MapOperator");

    auto input_type = operator_descs_[dag_->predecessor(op)].return_type;
    auto return_type = operator_descs_[op].return_type;
    const std::string functor_class = GenerateLlvmFunctor(
            context_, op->name(), op->llvm_ir, {input_type}, return_type->name);

    emitOperatorMake(var_name, "MapOperator", op, {}, {functor_class + "()"});
}

void CodeGenVisitor::operator()(DAGMaterializeRowVector *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MaterializeRowVectorOperator");

    auto input_type = operator_descs_[dag_->predecessor(op)].return_type;
    emitOperatorMake(var_name, "MaterializeRowVectorOperator", op,
                     {input_type->name}, {});
}

void CodeGenVisitor::operator()(DAGPipeline * /*op*/) {
    throw std::runtime_error("CodeGen encountered pipeline operator.");
}

void CodeGenVisitor::operator()(DAGParameterLookup *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ConstantTupleOperator");

    const auto input_arg = "input_" + std::to_string(dag_->input_port(op));

    emitOperatorMake(var_name, "ConstantTupleOperator", op, {}, {input_arg});
}

void CodeGenVisitor::operator()(DAGPartition *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "PartitionOperator");

    emitOperatorMake(var_name, "PartitionOperator", op, {}, {"256"});
}

void CodeGenVisitor::operator()(DAGProjection *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MapOperator");

    auto input_type = operator_descs_[dag_->predecessor(op)].return_type;
    auto return_type = operator_descs_[op].return_type;

    const auto functor_class =
            context_->GenerateSymbolName(var_name + "_func", true);

    std::vector<std::string> call_args;
    for (auto const pos : op->positions) {
        call_args.emplace_back("t." + input_type->names[pos]);
    }

    context_->definitions() <<  //
            format("class %s {"
                   "public:"
                   "    auto operator()(const %s &t) {"
                   "        return %s{%s};"
                   "    }"
                   "};") %
                    functor_class % input_type->name % return_type->name %
                    join(call_args, ",");

    emitOperatorMake(var_name, "MapOperator", op, {}, {functor_class + "()"});
}

void CodeGenVisitor::operator()(DAGReduce *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ReduceOperator");

    auto return_type = operator_descs_[op].return_type;
    const std::string functor_class =
            GenerateLlvmFunctor(context_, op->name(), op->llvm_ir,
                                {return_type, return_type}, return_type->name);

    emitOperatorMake(var_name, "ReduceOperator", op, {},
                     {functor_class + "()"});
}

void CodeGenVisitor::operator()(DAGRange *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "RangeSourceOperator");

    emitOperatorMake(var_name, "RangeSourceOperator", op, {}, {});
};

void CodeGenVisitor::operator()(DAGEnsureSingleTuple *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "EnsureSingleTupleOperator");

    emitOperatorMake(var_name, "EnsureSingleTupleOperator", op, {}, {});
};

void CodeGenVisitor::operator()(DAGFilter *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "FilterOperator");

    auto input_type = operator_descs_[dag_->predecessor(op)].return_type;
    const std::string functor_class = GenerateLlvmFunctor(
            context_, op->name(), op->llvm_ir, {input_type}, "bool");

    emitOperatorMake(var_name, "FilterOperator", op, {},
                     {functor_class + "()"});
};

void CodeGenVisitor::operator()(DAGJoin *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "JoinOperator");

    // Build key and value types
    // TODO(sabir): This currently only works for keys of size 1
    const auto up1Type = dag_->predecessor(op, 0)->tuple->type;
    const auto up2Type = dag_->predecessor(op, 1)->tuple->type;
    auto key_Tuple = up1Type->ComputeHeadTuple();

    auto key_type = EmitTupleStructDefinition(context_, key_Tuple);

    auto value_tuple1 = up1Type->ComputeTailTuple();
    auto value_type1 = EmitTupleStructDefinition(context_, value_tuple1);

    auto value_tuple2 = up2Type->ComputeTailTuple();
    auto value_type2 = EmitTupleStructDefinition(context_, value_tuple2);

    // Build operator
    std::vector<std::string> template_args = {key_type->name, value_type1->name,
                                              value_type2->name};

    emitOperatorMake(var_name, "JoinOperator", op, template_args);
};

void CodeGenVisitor::operator()(DAGCartesian *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "CartesianOperator");
    std::string pred2Tuple =
            operator_descs_[dag_->predecessor(op, 1)].return_type->name;
    std::vector<std::string> template_args = {pred2Tuple};
    emitOperatorMake(var_name, "CartesianOperator", op, template_args);
};

void CodeGenVisitor::operator()(DAGParallelMap *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ParallelMapOperator");

    // Call nested code gen
    const auto inner_plan =
            GenerateExecuteTuples(dag_->inner_dag(op), context_);

    emitOperatorMake(var_name, "ParallelMapOperator", op, {},
                     {inner_plan.name});
}

void CodeGenVisitor::operator()(DAGSplitColumnData *const op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SplitColumnDataOperator");

    emitOperatorMake(var_name, "SplitColumnDataOperator", op, {},
                     {"omp_get_num_threads()"});
}

void CodeGenVisitor::operator()(DAGSplitRowData *const op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SplitRowDataOperator");

    emitOperatorMake(var_name, "SplitRowDataOperator", op, {},
                     {"omp_get_num_threads()"});
}

void CodeGenVisitor::operator()(DAGSplitRange *const op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SplitRangeOperator");

    emitOperatorMake(var_name, "SplitRangeOperator", op, {},
                     {"omp_get_num_threads()"});
}

void CodeGenVisitor::visit_reduce_by_key(DAGOperator *op,
                                         const std::string &operator_name) {
    assert(dynamic_cast<DAGReduceByKey *>(op) != nullptr ||
           dynamic_cast<DAGReduceByKeyGrouped *>(op) != nullptr);

    const std::string var_name =
            CodeGenVisitor::visit_common(op, operator_name);

    // Build key and value types

    // TODO(sabir): This currently only works for keys of size 1
    auto key_type_tuple = op->tuple->type->ComputeHeadTuple();
    auto key_type = EmitTupleStructDefinition(context_, key_type_tuple);

    auto value_type_tuple = op->tuple->type->ComputeTailTuple();
    auto value_type = EmitTupleStructDefinition(context_, value_type_tuple);

    // Construct functor
    const std::string functor_class =
            GenerateLlvmFunctor(context_, op->name(), op->llvm_ir,
                                {value_type, value_type}, value_type->name);

    // Collect template arguments
    std::vector<std::string> template_args = {key_type->name, value_type->name};

    // Generate call
    emitOperatorMake(var_name, operator_name, op, template_args,
                     {functor_class + "()"});
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

    auto output_type = EmitTupleStructDefinition(context_, op->tuple->type);

    operator_descs_.emplace(op, OperatorDesc{var_name, output_type});
    context_->includes().insert("\"" + operator_name + ".h\"");
    return var_name;
}

void CodeGenVisitor::emitOperatorMake(
        const std::string &variable_name, const std::string &operator_name,
        const DAGOperator *const op,
        const std::vector<std::string> &extra_template_args,
        const std::vector<std::string> &extra_args) {
    // First template argument: current tuple
    auto return_type = operator_descs_[op].return_type;
    std::vector<std::string> template_args = {return_type->name};

    // Take over extra template arguments
    template_args.insert(template_args.end(), extra_template_args.begin(),
                         extra_template_args.end());

    // Default input arguments: references to predecessors
    std::vector<std::string> args;
    for (size_t i = 0; i < op->num_in_ports(); i++) {
        const auto pred = dag_->predecessor(op, i);
        const auto arg = "&" + operator_descs_[pred].var_name;
        args.emplace_back(arg);
    }

    // Take over extra arguments
    args.insert(args.end(), extra_args.begin(), extra_args.end());

    // Generate call
    plan_body_ << format("auto %s = make%s<%s>(%s);") % variable_name %
                          operator_name % join(template_args, ",") %
                          join(args, ",");
}

}  // namespace cpp
}  // namespace code_gen
