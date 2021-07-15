#include "parallelize_concurrent_single_inout.hpp"

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

void ParallelizeConcurrentSingleInout::Run(
        DAG *const dag, const std::string & /*config*/) const {
    for (auto *const op : dag->operators()) {
        if (!IsInstanceOf<DAGConcurrentExecute>(op)) continue;
        auto *const pop = dynamic_cast<DAGConcurrentExecute *>(op);
        assert(pop != nullptr);
        auto *const inner_dag = dag->inner_dag(pop);

        /*
         * Merge inputs into single tuple
         */

        // Collect inputs, then reset them
        std::vector<std::pair<int, DAG::FlowTip>> broadcasted_inputs;
        std::vector<std::pair<int, DAG::FlowTip>> partitioned_inputs;

        for (auto const &input : inner_dag->inputs()) {
            auto *const param_op = input.second.op;
            auto const out_flow = inner_dag->out_flow(param_op);
            if (IsInstanceOf<DAGBroadcast>(out_flow.target.op)) {
                broadcasted_inputs.emplace_back(input);
            } else {
                partitioned_inputs.emplace_back(input);
            }
        }

        inner_dag->reset_inputs();

        // Collect in-flows, then reset them
        std::unordered_map<int, DAG::Flow> in_flows;
        for (auto const f : dag->in_flows(pop)) {
            auto const [it, has_inserted] = in_flows.emplace(f.target.port, f);
            assert(has_inserted);
        }
        for (auto const [port, f] : in_flows) {
            dag->RemoveFlow(f);
        }

        // Build in-flows from scratch, combining the original inputs into one
        auto *const zip_partitioned_op = new DAGZip();
        dag->AddOperator(zip_partitioned_op);

        for (auto const input : partitioned_inputs) {
            auto const in_flow = in_flows.at(input.first);
            dag->AddFlow(in_flow.source, zip_partitioned_op,
                         zip_partitioned_op->num_inputs++);
        }

        auto *const zip_broadcasted_op = new DAGZip();
        dag->AddOperator(zip_broadcasted_op);

        for (auto const input : broadcasted_inputs) {
            auto const in_flow = in_flows.at(input.first);
            dag->AddFlow(in_flow.source, zip_broadcasted_op,
                         zip_broadcasted_op->num_inputs++);
        }

        auto *const cartesian_op = new DAGCartesian();
        dag->AddOperator(cartesian_op);

        dag->AddFlow(zip_partitioned_op, cartesian_op, 0);
        dag->AddFlow(zip_broadcasted_op, cartesian_op, 1);
        dag->AddFlow(cartesian_op, pop);

        pop->num_inputs = 1;

        // Adapt how inner plan reads inputs
        size_t prefix_width = 0;
        for (auto const input : partitioned_inputs) {
            auto const out_flow = inner_dag->out_flow(input.second.op);
            auto *const param_op = out_flow.source.op;

            inner_dag->add_input(0, param_op);
            inner_dag->RemoveFlow(out_flow);

            auto *const projection_op = new DAGProjection();
            inner_dag->AddOperator(projection_op);
            auto const width = param_op->tuple->type->field_types.size();
            boost::copy(boost::irange(prefix_width, prefix_width + width),
                        std::back_inserter(projection_op->positions));
            prefix_width += width;

            inner_dag->AddFlow(out_flow.source, projection_op);
            inner_dag->AddFlow(projection_op, out_flow.target);
        }

        for (auto const input : broadcasted_inputs) {
            auto const out_flow = inner_dag->out_flow(input.second.op);
            auto *const param_op = out_flow.source.op;
            auto *const bc_op = out_flow.target.op;
            auto const next_flow = inner_dag->out_flow(bc_op);

            inner_dag->add_input(0, param_op);
            inner_dag->RemoveFlow(out_flow);
            inner_dag->RemoveFlow(next_flow);
            inner_dag->RemoveOperator(bc_op);

            auto *const projection_op = new DAGProjection();
            inner_dag->AddOperator(projection_op);
            auto const width = param_op->tuple->type->field_types.size();
            boost::copy(boost::irange(prefix_width, prefix_width + width),
                        std::back_inserter(projection_op->positions));
            prefix_width += width;

            inner_dag->AddFlow(out_flow.source, projection_op);
            inner_dag->AddFlow(projection_op, next_flow.target);
        }

        /*
         * Add materialization operator at the end
         */
        assert(inner_dag->out_degree() == 1);

        // Add materialization operator to inner DAG
        auto *const config_op = new DAGConstantTuple();
        inner_dag->AddOperator(config_op);
        const auto *const output_type = dag::type::Tuple::MakeTuple(
                {dag::type::Atomic::MakeAtomic("std::string")});
        config_op->tuple =
                isocpp_p0201::make_polymorphic_value<dag::collection::Tuple>(
                        output_type);
        config_op->values = {
                "R\"STRING("
                "   {"
                "       \"filename\" : \"/tmp/jitq-query-result-%1$d.parquet\","
                "       \"filesystem\" : \"file\""
                "   }"
                ")STRING\""};

        auto *const expand_pattern_op = new DAGExpandPattern();
        inner_dag->AddOperator(expand_pattern_op);

        auto *const worker_id_op = new DAGConstantTuple();
        inner_dag->AddOperator(worker_id_op);

        const auto *const worker_id_type = dag::type::Tuple::MakeTuple(
                {dag::type::Atomic::MakeAtomic("long")});
        worker_id_op->tuple =
                isocpp_p0201::make_polymorphic_value<dag::collection::Tuple>(
                        worker_id_type);
        worker_id_op->values = {"$WID"};

        auto *const mat_columns_op = new DAGMaterializeColumnChunks();
        inner_dag->AddOperator(mat_columns_op);

        auto *const parquet_mat_op = new DAGMaterializeParquet();
        inner_dag->AddOperator(parquet_mat_op);

        for (size_t i = 0; i < pop->tuple->type->field_types.size(); i++) {
            parquet_mat_op->column_names.push_back(
                    (boost::format("v%1%") % i).str());
        }

        inner_dag->AddFlow(inner_dag->output(), mat_columns_op);
        inner_dag->AddFlow(mat_columns_op, parquet_mat_op, 0);
        inner_dag->AddFlow(config_op, expand_pattern_op, 0);
        inner_dag->AddFlow(worker_id_op, expand_pattern_op, 1);
        inner_dag->AddFlow(expand_pattern_op, parquet_mat_op, 1);
        inner_dag->set_output(parquet_mat_op);

        // Add scan operator to outer DAG
        auto *const parquet_scan_op = new DAGParquetScan();
        dag->AddOperator(parquet_scan_op);
        parquet_scan_op->filesystem = "file";

        auto *const column_scan_op = new DAGColumnScan();
        dag->AddOperator(column_scan_op);

        // Compute Parquet operator types
        std::vector<const dag::type::FieldType *> column_types;
        for (auto const &type : pop->tuple->type->field_types) {
            column_types.emplace_back(dag::type::Array::MakeArray(
                    dag::type::Tuple::MakeTuple({type}),
                    dag::type::ArrayLayout::kC, 1));
        }
        const auto *const parquet_output_type =
                dag::type::Tuple::MakeTuple(column_types);
        parquet_scan_op->tuple =
                isocpp_p0201::make_polymorphic_value<dag::collection::Tuple>(
                        parquet_output_type);

        // Compute Parquet operator column indices and filters
        auto const width = column_types.size();
        boost::copy(boost::irange(0UL, width),
                    std::back_inserter(parquet_scan_op->column_indexes));
        using RangeFilters = DAGParquetScan::RangeFilters;
        parquet_scan_op->column_range_filters =
                std::vector<RangeFilters>(width, RangeFilters{});

        // Reconnect outer DAG
        auto const out_flow = dag->out_flow(pop);
        dag->RemoveFlow(out_flow);
        dag->AddFlow(pop, parquet_scan_op);
        dag->AddFlow(parquet_scan_op, column_scan_op);
        dag->AddFlow(column_scan_op, out_flow.target);
    }
}

}  // namespace optimize
