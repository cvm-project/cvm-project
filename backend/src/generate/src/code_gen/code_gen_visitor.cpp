#include "code_gen_visitor.hpp"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/mpl/list.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "code_gen.hpp"
#include "dag/collection/array.hpp"
#include "dag/collection/atomic.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/type/array.hpp"
#include "utils/visitor.hpp"

using boost::format;
using boost::algorithm::join;

namespace code_gen::cpp {

void CodeGenVisitor::operator()(DAGAntiJoin *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "AntiJoinOperator");

    // Build key and value types
    // TODO(sabir): This currently only works for keys of size 1
    const auto up1Type = dag_->predecessor(op, 0)->tuple->type;
    auto key_Tuple = up1Type->ComputeHeadTuple();

    auto key_type = EmitTupleStructDefinition(context_, key_Tuple);

    auto value_tuple1 = up1Type->ComputeTailTuple();
    auto value_type1 = EmitTupleStructDefinition(context_, value_tuple1);

    // Build operator
    std::vector<std::string> template_args = {key_type->name,
                                              value_type1->name};

    emitOperatorMake(var_name, "AntiJoinOperator", op, template_args);
};

void CodeGenVisitor::operator()(DAGAntiJoinPredicated *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "AntiJoinPredicatedOperator");

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

    auto input_type1 = operator_descs_[dag_->predecessor(op, 0)].return_type;
    auto input_type2 = operator_descs_[dag_->predecessor(op, 1)].return_type;
    const std::string functor_class =
            GenerateLlvmFunctor(context_, op->name(), op->llvm_ir,
                                {input_type1, input_type2}, "bool");

    emitOperatorMake(var_name, "AntiJoinPredicatedOperator", op, template_args,
                     {functor_class + "()"});
};

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

void CodeGenVisitor::operator()(DAGMaterializeColumnChunks *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MaterializeColumnChunksOperator");

    GenerateTupleToValue(context_, op->tuple->type);

    emitOperatorMake(var_name, "MaterializeColumnChunksOperator", op, {}, {});
}

void CodeGenVisitor::operator()(DAGMaterializeParquet *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MaterializeParquetOperator");

    GenerateValueToTuple(context_, op->tuple->type);

    std::vector<std::string> column_names;
    for (auto const &n : op->column_names) {
        column_names.emplace_back((format("\"%1%\"") % n).str());
    }

    const std::string column_names_expression =
            (format("{%1%}") % join(column_names, ",")).str();

    emitOperatorMake(var_name, "MaterializeParquetOperator", op, {},
                     {column_names_expression});
}

void CodeGenVisitor::operator()(DAGMaterializeRowVector *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MaterializeRowVectorOperator");

    emitOperatorMake(var_name, "MaterializeRowVectorOperator", op, {}, {});
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

    emitOperatorMake(var_name, "PartitionOperator", op, {}, {"16"});
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

void CodeGenVisitor::operator()(DAGExpandPattern *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ExpandPatternOperator");

    GenerateTupleToValue(context_, dag_->predecessor(op, 0)->tuple->type);

    emitOperatorMake(var_name, "ExpandPatternOperator", op, {}, {});
}

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
    const auto up1Type = dag_->predecessor(op, 0)->tuple->type;
    const auto up2Type = dag_->predecessor(op, 1)->tuple->type;

    auto key_Tuple = up1Type->ComputeHeadTuple(op->num_keys);

    auto key_type = EmitTupleStructDefinition(context_, key_Tuple);

    auto value_tuple1 = up1Type->ComputeTailTuple(op->num_keys);
    auto value_type1 = EmitTupleStructDefinition(context_, value_tuple1);

    auto value_tuple2 = up2Type->ComputeTailTuple(op->num_keys);
    auto value_type2 = EmitTupleStructDefinition(context_, value_tuple2);

    // Build operator
    std::vector<std::string> template_args = {key_type->name, value_type1->name,
                                              value_type2->name,
                                              std::to_string(op->num_keys)};

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

void CodeGenVisitor::operator()(DAGParallelMapOmp *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ParallelMapOmpOperator");

    // Call nested code gen
    const auto inner_plan =
            GenerateExecuteTuples(dag_->inner_dag(op), context_);

    emitOperatorMake(var_name, "ParallelMapOmpOperator", op, {},
                     {inner_plan.name});
}

void CodeGenVisitor::operator()(DAGParquetScan *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ParquetScanOperator");

    GenerateTupleToValue(context_, dag_->predecessor(op)->tuple->type);
    GenerateValueToTuple(context_, op->tuple->type);

    const auto &column_types = op->tuple->type->field_types;
    const auto &range_filters = op->column_range_filters;
    assert(range_filters.size() == column_types.size());
    assert(range_filters.size() == op->column_indexes.size());

    std::vector<std::string> predicates;
    std::vector<std::string> column_type_names;
    std::vector<std::string> column_ids;

    for (size_t i = 0; i < range_filters.size(); i++) {
        auto const &ranges = range_filters[i];
        auto const column_type =
                dynamic_cast<const dag::type::Array *>(column_types[i]);
        assert(column_type != nullptr);
        auto const item_type = dynamic_cast<const dag::type::Atomic *>(
                column_type->tuple_type->field_types.at(0));
        assert(item_type != nullptr);
        std::vector<std::string> column_predicates;
        column_predicates.reserve(ranges.size());
        for (auto const &r : ranges) {
            column_predicates.push_back(
                    (format("runtime::operators::MakeRangePredicate("
                            "       static_cast<%1%>(%2%),"
                            "       static_cast<%1%>(%3%))") %
                     item_type->type % r.first % r.second)
                            .str());
        }
        predicates.push_back(
                (format("{%1%}") % join(column_predicates, ",")).str());
        column_type_names.push_back("\"" + item_type->type + "\"");
        column_ids.push_back(std::to_string(op->column_indexes[i]));
    }

    auto const predicates_expression =
            (format("{%1%}") % join(predicates, ",")).str();
    auto const column_types_expression =
            (format("{%1%}") % join(column_type_names, ",")).str();
    auto const column_ids_expression =
            (format("{%1%}") % join(column_ids, ",")).str();
    auto const filesystem = (format("\"%1%\"") % op->filesystem).str();

    emitOperatorMake(var_name, "ParquetScanOperator", op, {},
                     {predicates_expression, column_types_expression,
                      column_ids_expression, filesystem});
}

void CodeGenVisitor::operator()(DAGSemiJoin *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SemiJoinOperator");

    // Build key and value types
    // TODO(sabir): This currently only works for keys of size 1
    const auto up1Type = dag_->predecessor(op, 0)->tuple->type;
    auto key_Tuple = up1Type->ComputeHeadTuple();

    auto key_type = EmitTupleStructDefinition(context_, key_Tuple);

    auto value_tuple1 = up1Type->ComputeTailTuple();
    auto value_type1 = EmitTupleStructDefinition(context_, value_tuple1);

    // Build operator
    std::vector<std::string> template_args = {key_type->name,
                                              value_type1->name};

    emitOperatorMake(var_name, "SemiJoinOperator", op, template_args);
};

void CodeGenVisitor::operator()(DAGSplitColumnData *const op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SplitColumnDataOperator");

    emitOperatorMake(var_name, "SplitColumnDataOperator", op, {}, {});
}

void CodeGenVisitor::operator()(DAGSplitRowData *const op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SplitRowDataOperator");

    emitOperatorMake(var_name, "SplitRowDataOperator", op, {}, {});
}

void CodeGenVisitor::operator()(DAGSplitRange *const op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SplitRangeOperator");

    emitOperatorMake(var_name, "SplitRangeOperator", op, {}, {});
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

void CodeGenVisitor::operator()(DAGReduceByIndex *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ReduceByIndexOperator");

    // Build key and value types
    const auto key_type_tuple = op->tuple->type->ComputeHeadTuple();
    const auto key_type = EmitTupleStructDefinition(context_, key_type_tuple);

    const auto value_type_tuple = op->tuple->type->ComputeTailTuple();
    const auto value_type =
            EmitTupleStructDefinition(context_, value_type_tuple);

    // Construct functor
    const std::string functor_class =
            GenerateLlvmFunctor(context_, op->name(), op->llvm_ir,
                                {value_type, value_type}, value_type->name);

    // Collect template arguments
    std::vector<std::string> template_args = {key_type->name, value_type->name,
                                              std::to_string(op->min),
                                              std::to_string(op->max)};

    // Generate call with parameters
    emitOperatorMake(var_name, "ReduceByIndexOperator", op, template_args,
                     {functor_class + "()"});
}

void CodeGenVisitor::operator()(DAGTopK *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "TopKOperator");
    emitOperatorMake(var_name, "TopKOperator", op, {},
                     {std::to_string(op->num_elements)});
}

void CodeGenVisitor::operator()(DAGSort *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "SortOperator");
    emitOperatorMake(var_name, "SortOperator", op, {}, {});
}

void CodeGenVisitor::operator()(DAGZip *const op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ZipOperator");

    emitOperatorMake(var_name, "ZipOperator", op, {}, {});
}

void CodeGenVisitor::operator()(DAGOperator *const op) {
    throw std::runtime_error("CodeGen encountered unknown operator type: " +
                             op->name());
}

// TODO(ingo): This could be an independent visitor
auto CodeGenVisitor::visit_common(DAGOperator *op,
                                  const std::string &operator_name)
        -> std::string {
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

}  // namespace code_gen::cpp
