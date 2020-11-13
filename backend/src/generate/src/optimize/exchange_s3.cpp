#include "exchange_s3.hpp"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/format.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/irange.hpp>

#include "dag/collection/tuple.hpp"
#include "dag/dag.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/type/array.hpp"
#include "dag/type/atomic.hpp"
#include "dag/type/tuple.hpp"
#include "dag/utils/type_traits.hpp"

using dag::utils::IsInstanceOf;

namespace optimize {

void ExchangeS3::Run(DAG *const dag, const std::string & /*config*/) const {
    for (auto *const pop_candidate : dag->operators()) {
        if (!IsInstanceOf<DAGConcurrentExecute>(pop_candidate)) continue;
        auto *const pop = dynamic_cast<DAGConcurrentExecute *>(pop_candidate);
        assert(pop != nullptr);
        auto *const pop_inner_dag = dag->inner_dag(pop);

        // Replace each DAGExchange with a DAGPartitionedExchange of one level
        std::vector<DAGExchange *> exchange_operators;
        for (auto *const op : pop_inner_dag->operators()) {
            if (IsInstanceOf<DAGExchange>(op)) {
                exchange_operators.push_back(dynamic_cast<DAGExchange *>(op));
            }
        }
        for (auto *const op : exchange_operators) {
            auto *const new_op = new DAGPartitionedExchange();
            new_op->tuple = op->tuple;
            pop_inner_dag->ReplaceOperator(op, new_op);
            new_op->num_levels = 1;
            new_op->level_num = 0;
        }

        // Replace each DAGPartitionedExchange with a sequence including a
        // DAGExchangeS3 operator
        std::vector<DAGPartitionedExchange *> partitioned_exchange_operators;
        for (auto *const op : pop_inner_dag->operators()) {
            if (IsInstanceOf<DAGPartitionedExchange>(op)) {
                partitioned_exchange_operators.push_back(
                        dynamic_cast<DAGPartitionedExchange *>(op));
            }
        }

        for (auto *const op : partitioned_exchange_operators) {
            auto *const dag = pop_inner_dag;
            auto const in_flow = dag->in_flow(op);
            auto const out_flow = dag->out_flow(op);

            // Create new operators in outer DAG
            auto *const partition_op = new DAGPartition();
            dag->AddOperator(partition_op);

            // Create degree-of-parallelism operator
            auto *const part_dop_op = new DAGConstantTuple();
            dag->AddOperator(part_dop_op);
            part_dop_op->values.emplace_back(std::to_string(op->num_levels));
            part_dop_op->values.emplace_back(std::to_string(op->level_num));
            part_dop_op->values.emplace_back("$DOP");
            part_dop_op->values.emplace_back("$WID");
            part_dop_op->tuple =
                    jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                            dag::type::Tuple::MakeTuple(
                                    {dag::type::Atomic::MakeAtomic("long"),
                                     dag::type::Atomic::MakeAtomic("long"),
                                     dag::type::Atomic::MakeAtomic("long"),
                                     dag::type::Atomic::MakeAtomic("long")}));

            auto *const map_op = new DAGMapCpp();
            dag->AddOperator(map_op);
            map_op->function_name = "runtime::operators::ComputeGroupSizeTuple";
            map_op->tuple =
                    jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                            dag::type::Tuple::MakeTuple(
                                    {dag::type::Atomic::MakeAtomic("long")}));

            auto *const group_by_op = new DAGGroupBy();
            dag->AddOperator(group_by_op);

            auto *const nested_map_op = new DAGNestedMap();
            dag->AddOperator(nested_map_op);

            auto *const row_scan_op = new DAGRowScan();
            dag->AddOperator(row_scan_op);

            auto *const exchange_op = new DAGExchangeS3();
            dag->AddOperator(exchange_op);
            exchange_op->num_levels = op->num_levels;
            exchange_op->level_num = op->level_num;

            // Create degree-of-parallelism operator
            auto *const exchange_dop_op = new DAGConstantTuple();
            dag->AddOperator(exchange_dop_op);
            exchange_dop_op->values.emplace_back("$DOP");
            exchange_dop_op->tuple =
                    jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                            dag::type::Tuple::MakeTuple(
                                    {dag::type::Atomic::MakeAtomic("long")}));

            // Create worker-ID operator
            auto *const exchange_wid_op = new DAGConstantTuple();
            dag->AddOperator(exchange_wid_op);
            exchange_wid_op->values.emplace_back("$WID");
            exchange_wid_op->tuple =
                    jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                            dag::type::Tuple::MakeTuple(
                                    {dag::type::Atomic::MakeAtomic("long")}));

            auto *const parquet_scan_op = new DAGParquetScan();
            dag->AddOperator(parquet_scan_op);
            parquet_scan_op->filesystem = "s3";

            auto *const column_scan_op = new DAGColumnScan();
            dag->AddOperator(column_scan_op);

            // Compute Parquet operator types
            std::vector<const dag::type::FieldType *> column_types;
            for (auto const &type : op->tuple->type->field_types) {
                column_types.emplace_back(dag::type::Array::MakeArray(
                        dag::type::Tuple::MakeTuple({type}),
                        dag::type::ArrayLayout::kC, 1));
            }
            const auto *const output_type =
                    dag::type::Tuple::MakeTuple(column_types);
            parquet_scan_op->tuple =
                    jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                            output_type);

            // Compute Parquet operator column indices and filters
            auto const width = column_types.size();
            boost::copy(boost::irange(0UL, width),
                        std::back_inserter(parquet_scan_op->column_indexes));
            using RangeFilters = DAGParquetScan::RangeFilters;
            parquet_scan_op->column_range_filters =
                    std::vector<RangeFilters>(width, RangeFilters{});

            // Reconnect outer DAG
            dag->RemoveFlow(in_flow);
            dag->RemoveFlow(out_flow);
            dag->RemoveOperator(op);

            dag->AddFlow(in_flow.source, partition_op, 0);
            dag->AddFlow(part_dop_op, map_op);
            dag->AddFlow(map_op, partition_op, 1);
            dag->AddFlow(partition_op, group_by_op);
            dag->AddFlow(group_by_op, nested_map_op);
            dag->AddFlow(nested_map_op, row_scan_op);
            dag->AddFlow(row_scan_op, exchange_op, 0);
            dag->AddFlow(exchange_dop_op, exchange_op, 1);
            dag->AddFlow(exchange_wid_op, exchange_op, 2);
            dag->AddFlow(exchange_op, parquet_scan_op);
            dag->AddFlow(parquet_scan_op, column_scan_op);
            dag->AddFlow(column_scan_op, out_flow.target);

            // Create new operators for inner DAG
            dag->set_inner_dag(nested_map_op, new DAG());
            auto *const inner_dag = dag->inner_dag(nested_map_op);

            auto *const id_param_op = new DAGParameterLookup();
            inner_dag->AddOperator(id_param_op);

            auto *const id_projection_op = new DAGProjection();
            inner_dag->AddOperator(id_projection_op);
            id_projection_op->positions = {0};

            auto *const data_param_op = new DAGParameterLookup();
            inner_dag->AddOperator(data_param_op);

            auto *const data_projection_op = new DAGProjection();
            inner_dag->AddOperator(data_projection_op);
            data_projection_op->positions = {1};

            auto *const scan_op1 = new DAGRowScan();
            inner_dag->AddOperator(scan_op1);

            auto *const scan_op2 = new DAGRowScan();
            inner_dag->AddOperator(scan_op2);

            auto *const mat_columns_op = new DAGMaterializeColumnChunks();
            inner_dag->AddOperator(mat_columns_op);

            auto *const cartesian_op = new DAGCartesian();
            inner_dag->AddOperator(cartesian_op);

            auto *const mat_rows_op = new DAGMaterializeRowVector();
            inner_dag->AddOperator(mat_rows_op);

            // Connect inner DAG
            inner_dag->AddFlow(id_param_op, id_projection_op);
            inner_dag->AddFlow(id_projection_op, cartesian_op, 0);
            inner_dag->AddFlow(data_param_op, data_projection_op);
            inner_dag->AddFlow(data_projection_op, scan_op1);
            inner_dag->AddFlow(scan_op1, scan_op2);
            inner_dag->AddFlow(scan_op2, mat_columns_op);
            inner_dag->AddFlow(mat_columns_op, cartesian_op, 1);
            inner_dag->AddFlow(cartesian_op, mat_rows_op);

            inner_dag->set_input(0, id_param_op);
            inner_dag->add_input(0, data_param_op);
            inner_dag->set_output(mat_rows_op);
        }
    }
}

}  // namespace optimize
