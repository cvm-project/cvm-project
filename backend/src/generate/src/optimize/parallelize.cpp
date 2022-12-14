#include "parallelize.hpp"

#include <queue>

#include <boost/mpl/list.hpp>
#include <polymorphic_value.h>

#include "dag/collection/tuple.hpp"
#include "dag/dag.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/type/atomic.hpp"
#include "dag/type/tuple.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "dag/utils/type_traits.hpp"
#include "optimize/utils.hpp"
#include "utils/visitor.hpp"

using dag::utils::IsInstanceOf;
using dag::utils::IsSingleTupleProducer;
using dag::utils::IsSourceOperator;

namespace optimize {

struct CollectSourcesVisitor
    : public Visitor<CollectSourcesVisitor, DAGOperator,
                     boost::mpl::list<DAGOperator>> {
    void operator()(DAGOperator *const op) {
        if (IsSourceOperator(op)) {
            sources_.emplace_back(op);
        }
    }
    std::deque<DAGOperator *> sources_;
};

auto StartParallelMap(DAG *const dag, DAGOperator *const source_op)
        -> DAGParallelMap * {
    assert(dag->in_degree(source_op) == 1);
    assert(dag->out_degree(source_op) == 1);

    auto *const pred_op = dag->predecessor(source_op);
    auto *const next_op = dag->successor(source_op);
    auto const in_flow = dag->in_flow(source_op);
    auto const out_flow = dag->out_flow(source_op);

    // Create split operator
    auto *const split_op = MakeSplitOperator(source_op);
    dag->AddOperator(split_op);

    // Create degree-of-parallelism operator
    auto *const dop_op = new DAGConstantTuple();
    dag->AddOperator(dop_op);
    dop_op->values.emplace_back("$DOP");
    dop_op->tuple =
            isocpp_p0201::make_polymorphic_value<dag::collection::Tuple>(
                    dag::type::Tuple::MakeTuple(
                            {dag::type::Atomic::MakeAtomic("long")}));

    // Create parallelize operator
    auto *const pop = new DAGParallelMap();
    dag->AddOperator(pop);
    dag->set_inner_dag(pop, new DAG());
    auto *const inner_dag = dag->inner_dag(pop);

    // Fill the parallelize operator with a parameter lookup
    auto *const param_op = new DAGParameterLookup();
    inner_dag->AddOperator(param_op);
    inner_dag->set_input(param_op);

    // Move source operator into parallelize operator
    dag->RemoveFlow(in_flow);
    dag->RemoveFlow(out_flow);
    dag->MoveOperator(inner_dag, source_op);
    inner_dag->AddFlow(param_op, source_op);
    inner_dag->set_output(source_op);

    // Connect parallelize operator with old successor and predecessor
    dag->AddFlow(pred_op, split_op, 0);
    dag->AddFlow(dop_op, split_op, 1);
    dag->AddFlow(split_op, in_flow.source.port, pop);
    dag->AddFlow(pop, next_op, out_flow.target.port);

    return pop;
}

void Parallelize::Run(DAG *const dag, const std::string & /*config*/) const {
    // Collect all source operators
    CollectSourcesVisitor source_collector;
    dag::utils::ApplyInReverseTopologicalOrder(dag, source_collector.functor());
    auto &source_operators = source_collector.sources_;

    // Extend parallelize operators
    std::queue<DAGParallelMap *> parallelize_operators;
    while (true) {
        // If we don't have parallel subplans left to extend...
        if (parallelize_operators.empty()) {
            // ...look for source operator to start a new one
            while (!source_operators.empty()) {
                auto *const source_op = source_operators.front();
                source_operators.pop_front();
                if (dag->contains(source_op)) {
                    parallelize_operators.push(
                            StartParallelMap(dag, source_op));
                    break;
                }
            }
        }
        if (parallelize_operators.empty()) {
            break;
        }

        DAGParallelMap *const op = parallelize_operators.front();
        parallelize_operators.pop();

        auto *const inner_dag = dag->inner_dag(op);
        do {
            if (dag->out_degree(op) != 1) break;
            if (IsInstanceOf<DAGFilter,       //
                             DAGMap,          //
                             DAGParquetScan,  //
                             DAGColumnScan>(dag->successor(op))) {
                assert(dag->in_degree(op) == 1);
                auto *const other_op = dag->successor(op);
                auto *const next_op = dag->successor(other_op);

                auto const inner_flow = dag->out_flow(op);
                auto const out_flow = dag->out_flow(other_op);

                dag->RemoveFlow(inner_flow);
                dag->RemoveFlow(out_flow);
                dag->AddFlow(op, next_op, out_flow.target.port);

                dag->MoveOperator(inner_dag, other_op);
                inner_dag->AddFlow(inner_dag->output().op, other_op);
                inner_dag->set_output(other_op);

                continue;
            }
            if (IsInstanceOf<DAGReduce>(dag->successor(op))) {
                auto *const red_op = dag->successor(op);

                // Pre-reduce operator
                auto *const pre_reduction_op = new DAGReduce();
                inner_dag->AddOperator(pre_reduction_op);
                pre_reduction_op->llvm_ir = red_op->llvm_ir;

                auto *const ensure_single_tuple_op = new DAGEnsureSingleTuple();
                inner_dag->AddOperator(ensure_single_tuple_op);

                // Flows
                inner_dag->AddFlow(inner_dag->output().op, pre_reduction_op);
                inner_dag->AddFlow(pre_reduction_op, ensure_single_tuple_op);
                inner_dag->set_output(ensure_single_tuple_op);

                break;
            }
            if (IsInstanceOf<DAGReduceByKey>(dag->successor(op)) ||
                IsInstanceOf<DAGReduceByKeyGrouped>(dag->successor(op))) {
                auto *const red_op = dag->successor(op);
                auto *const next_op = dag->successor(red_op);

                auto const inner_flow = dag->out_flow(op);
                auto const out_flow = dag->out_flow(red_op);

                // Bypass original reduce in outer DAG
                dag->RemoveFlow(inner_flow);
                dag->RemoveFlow(out_flow);
                dag->AddFlow(op, next_op, out_flow.target.port);

                // Pre-reduce operator
                DAGOperator *pre_reduction_op{};
                if (IsInstanceOf<DAGReduceByKey>(red_op)) {
                    auto const vpr =
                            inner_dag->AddOperator(new DAGReduceByKey());
                    pre_reduction_op = inner_dag->to_operator(vpr);
                } else {
                    assert(IsInstanceOf<DAGReduceByKeyGrouped>(red_op));
                    auto const vpr =
                            inner_dag->AddOperator(new DAGReduceByKeyGrouped());
                    pre_reduction_op = inner_dag->to_operator(vpr);
                }
                pre_reduction_op->llvm_ir = red_op->llvm_ir;

                // Partition by group key
                auto *const part_op = new DAGPartition();
                inner_dag->AddOperator(part_op);

                // Create degree-of-parallelism operator
                auto *const dop_op = new DAGConstantTuple();
                inner_dag->AddOperator(dop_op);
                dop_op->values.emplace_back("$DOP");
                dop_op->tuple = isocpp_p0201::make_polymorphic_value<
                        dag::collection::Tuple>(dag::type::Tuple::MakeTuple(
                        {dag::type::Atomic::MakeAtomic("long")}));

                // End this parallel operator
                inner_dag->AddFlow(inner_dag->output().op, pre_reduction_op);
                inner_dag->AddFlow(pre_reduction_op, part_op, 0);
                inner_dag->AddFlow(dop_op, part_op, 1);
                inner_dag->set_output(part_op);

                // GroupBy operator after the first parallel map
                auto *const ex_op = new DAGGroupBy();
                dag->AddOperator(ex_op);

                auto *const proj_op = new DAGProjection();
                dag->AddOperator(proj_op);
                proj_op->positions = {1};

                // Next parallel map (for post reduction)
                auto *const next_pop = new DAGParallelMap();
                dag->AddOperator(next_pop);
                dag->set_inner_dag(next_pop, new DAG());
                auto *const next_inner_dag = dag->inner_dag(next_pop);

                // Reconnect outer level
                auto const outer_out_flow = dag->out_flow(op);
                dag->RemoveFlow(outer_out_flow);
                dag->AddFlow(op, ex_op);
                dag->AddFlow(ex_op, proj_op);
                dag->AddFlow(proj_op, next_pop);
                dag->AddFlow(next_pop, outer_out_flow.target.op,
                             outer_out_flow.target.port);

                // Start next parallel map
                auto *const next_param_op = new DAGParameterLookup();
                next_inner_dag->AddOperator(next_param_op);

                // Scan operators for (1) partitioning and (2) groupby
                auto *const scan_op1 = new DAGRowScan();
                next_inner_dag->AddOperator(scan_op1);

                auto *const scan_op2 = new DAGRowScan();
                next_inner_dag->AddOperator(scan_op2);

                // Original reduce as post-reduce operator
                dag->MoveOperator(next_inner_dag, red_op);

                // Connect new operators in next inner DAG
                next_inner_dag->set_input(next_param_op);
                next_inner_dag->AddFlow(next_param_op, scan_op1);
                next_inner_dag->AddFlow(scan_op1, scan_op2);
                next_inner_dag->AddFlow(scan_op2, red_op);
                next_inner_dag->set_output(red_op);

                // Remember the next parallel map such that it can be extended
                parallelize_operators.push(next_pop);

                break;
            }
            if (IsInstanceOf<DAGPartition>(dag->successor(op))) {
                auto *const partition_op = dag->successor(op);

                const auto partition_in_flow = dag->out_flow(op);
                assert(partition_in_flow.target.port == 0);
                const auto fanout_in_flow = dag->in_flow(partition_op, 1);
                const auto out_flow = dag->out_flow(partition_op);

                auto *const dop_op = fanout_in_flow.source.op;

                // Outer flows
                dag->RemoveFlow(partition_in_flow);
                dag->RemoveFlow(fanout_in_flow);
                dag->RemoveFlow(out_flow);
                dag->AddFlow(op, out_flow.target);

                // Move partition into parallel operator and connect
                dag->MoveOperator(inner_dag, partition_op);
                dag->MoveOperator(inner_dag, dop_op);

                inner_dag->AddFlow(inner_dag->output(), partition_op, 0);
                inner_dag->AddFlow(dop_op, partition_op, 1);

                inner_dag->set_output(partition_op);

                continue;
            }
            if (IsInstanceOf<DAGExpandPattern>(dag->successor(op))) {
                auto *const pattern_op = dag->successor(op);

                const auto parallel_in_flow = dag->in_flow(op);
                const auto pattern_in_flow = dag->out_flow(op);
                assert(pattern_in_flow.target.port == 1);
                const auto parameter_in_flow = dag->in_flow(pattern_op, 0);
                const auto out_flow = dag->out_flow(pattern_op);

                // New outer operators
                auto *const cartesian_op = new DAGCartesian();
                dag->AddOperator(cartesian_op);

                // Outer flows
                dag->RemoveFlow(parallel_in_flow);
                dag->RemoveFlow(pattern_in_flow);
                dag->RemoveFlow(parameter_in_flow);
                dag->RemoveFlow(out_flow);
                dag->AddFlow(parameter_in_flow.source, cartesian_op, 0);
                dag->AddFlow(parallel_in_flow.source, cartesian_op, 1);
                dag->AddFlow(cartesian_op, parallel_in_flow.target);
                dag->AddFlow(op, out_flow.target);

                // Remember existing inputs of inner DAG
                std::multimap<int, DAG::FlowTip> existing_inputs;
                for (const auto &input : inner_dag->inputs()) {
                    existing_inputs.insert(input);
                }

                // Move pattern into parallel operator and connect
                dag->MoveOperator(inner_dag, pattern_op);

                auto *const left_param_op = new DAGParameterLookup();
                inner_dag->AddOperator(left_param_op);

                auto *const left_proj_op = new DAGProjection();
                inner_dag->AddOperator(left_proj_op);
                left_proj_op->positions = {0};

                inner_dag->AddFlow(left_param_op, left_proj_op);
                inner_dag->AddFlow(left_proj_op, pattern_op, 0);
                inner_dag->AddFlow(inner_dag->output(), pattern_op, 1);

                inner_dag->set_input(0, left_param_op);
                inner_dag->set_output(pattern_op);

                // Connect existing inputs again
                for (const auto &input : existing_inputs) {
                    assert(input.first == 0);

                    auto *const right_param_op = input.second.op;
                    auto *const right_successor =
                            inner_dag->successor(right_param_op);

                    auto *const right_proj_op = new DAGProjection();
                    inner_dag->AddOperator(right_proj_op);
                    right_proj_op->positions = {1, 2, 3};  // XXX

                    inner_dag->RemoveFlow(inner_dag->out_flow(right_param_op));
                    inner_dag->AddFlow(right_param_op, right_proj_op);
                    inner_dag->AddFlow(right_proj_op, right_successor);
                    inner_dag->add_input(0, right_param_op);
                }

                continue;
            }
            if (IsInstanceOf<DAGJoin>(dag->successor(op))) {
                auto *const join_op =
                        reinterpret_cast<DAGJoin *>(dag->successor(op));

                auto const this_in_flow = dag->out_flow(op);
                auto const this_in_port = this_in_flow.target.port;
                auto const other_in_port = this_in_port == 0 ? 1 : 0;
                auto const other_in_flow = dag->in_flow(join_op, other_in_port);
                assert(this_in_port + other_in_port == 1);

                // (1) Prepare this side's input of the join

                // Partition this side's input by join key
                auto *const this_part_op = new DAGPartition();
                inner_dag->AddOperator(this_part_op);

                // Create degree-of-parallelism operator
                auto *const this_dop_op = new DAGConstantTuple();
                inner_dag->AddOperator(this_dop_op);
                this_dop_op->values.emplace_back("$DOP");
                this_dop_op->tuple = isocpp_p0201::make_polymorphic_value<
                        dag::collection::Tuple>(dag::type::Tuple::MakeTuple(
                        {dag::type::Atomic::MakeAtomic("long")}));

                // End this side's parallel map of the input
                inner_dag->AddFlow(inner_dag->output().op, this_part_op, 0);
                inner_dag->AddFlow(this_dop_op, this_part_op, 1);
                inner_dag->set_output(this_part_op);

                // GroupBy operator after this side's parallel map
                auto *const this_grouping_op = new DAGGroupBy();
                dag->AddOperator(this_grouping_op);

                // (2) Prepare other side's input of the join

                // Partition other side's input by join key
                auto *const other_part_op = new DAGPartition();
                dag->AddOperator(other_part_op);

                // Create degree-of-parallelism operator
                auto *const other_dop_op = new DAGConstantTuple();
                dag->AddOperator(other_dop_op);
                other_dop_op->values.emplace_back("$DOP");
                other_dop_op->tuple = isocpp_p0201::make_polymorphic_value<
                        dag::collection::Tuple>(dag::type::Tuple::MakeTuple(
                        {dag::type::Atomic::MakeAtomic("long")}));

                // GroupBy operator after other side's partitioning
                auto *const other_grouping_op = new DAGGroupBy();
                dag->AddOperator(other_grouping_op);

                // (3) Outer level join of partitions

                // Reconnect outer level before join
                dag->RemoveFlow(this_in_flow);
                dag->RemoveFlow(other_in_flow);

                dag->AddFlow(op, this_grouping_op);
                dag->AddFlow(this_grouping_op, join_op, this_in_port);

                dag->AddFlow(other_in_flow.source.op, other_in_flow.source.port,
                             other_part_op, 0);
                dag->AddFlow(other_dop_op, other_part_op, 1);
                dag->AddFlow(other_part_op, other_grouping_op);
                dag->AddFlow(other_grouping_op, join_op, other_in_port);

                // Project away the partition IDs
                auto *const proj_op = new DAGProjection();
                dag->AddOperator(proj_op);
                proj_op->positions = {1, 2};

                // Next parallel map (for the actual join)
                auto *const next_pop = new DAGParallelMap();
                dag->AddOperator(next_pop);
                dag->set_inner_dag(next_pop, new DAG());
                auto *const next_inner_dag = dag->inner_dag(next_pop);

                // Reconnect outer level after the join
                auto const out_flow = dag->out_flow(join_op);
                dag->RemoveFlow(out_flow);

                dag->AddFlow(join_op, proj_op);
                dag->AddFlow(proj_op, next_pop);
                dag->AddFlow(next_pop, out_flow.target.op,
                             out_flow.target.port);

                // (4) Fill the next parallel map with the actual join

                // Unzip the two inputs (of each partition)
                auto *const left_param_op = new DAGParameterLookup();
                next_inner_dag->AddOperator(left_param_op);

                auto *const right_param_op = new DAGParameterLookup();
                next_inner_dag->AddOperator(right_param_op);

                auto *const left_proj_op = new DAGProjection();
                next_inner_dag->AddOperator(left_proj_op);
                left_proj_op->positions = {0};

                auto *const right_proj_op = new DAGProjection();
                next_inner_dag->AddOperator(right_proj_op);
                right_proj_op->positions = {1};

                // Scan operators for (1) partitioning and (2) groupby
                auto *const left_scan_op1 = new DAGRowScan();
                next_inner_dag->AddOperator(left_scan_op1);

                auto *const left_scan_op2 = new DAGRowScan();
                next_inner_dag->AddOperator(left_scan_op2);

                auto *const right_scan_op1 = new DAGRowScan();
                next_inner_dag->AddOperator(right_scan_op1);

                auto *const right_scan_op2 = new DAGRowScan();
                next_inner_dag->AddOperator(right_scan_op2);

                // Use the original join operator to do the actual join
                auto *const inner_join_op = new DAGJoin();
                inner_join_op->num_keys = join_op->num_keys;
                join_op->num_keys = 1;
                next_inner_dag->AddOperator(inner_join_op);

                // Connect new operators in next inner DAG
                assert(next_inner_dag->in_degree() == 0);
                next_inner_dag->add_input(0, left_param_op);
                next_inner_dag->add_input(0, right_param_op);

                next_inner_dag->AddFlow(left_param_op, left_proj_op);
                next_inner_dag->AddFlow(left_proj_op, left_scan_op1);
                next_inner_dag->AddFlow(left_scan_op1, left_scan_op2);
                next_inner_dag->AddFlow(left_scan_op2, inner_join_op, 0);

                next_inner_dag->AddFlow(right_param_op, right_proj_op);
                next_inner_dag->AddFlow(right_proj_op, right_scan_op1);
                next_inner_dag->AddFlow(right_scan_op1, right_scan_op2);
                next_inner_dag->AddFlow(right_scan_op2, inner_join_op, 1);

                next_inner_dag->set_output(inner_join_op);

                // Remember the next parallel map such that it can be extended
                parallelize_operators.push(next_pop);

                break;
            }
            break;
        } while (true);

        if (IsSingleTupleProducer(inner_dag->output().op)) {
            continue;
        }

        // Add materialize operator at end of inner plan of parallel map
        auto *const mat_op = new DAGMaterializeRowVector();
        inner_dag->AddOperator(mat_op);

        inner_dag->AddFlow(inner_dag->output().op, mat_op);
        inner_dag->set_output(mat_op);

        // Add scan operator after parallel map to get back individual tuples
        auto *const next_op = dag->successor(op);
        auto const out_flow = dag->out_flow(op);

        auto *const sop = new DAGRowScan();
        dag->AddOperator(sop);

        dag->RemoveFlow(out_flow);
        dag->AddFlow(op, sop);
        dag->AddFlow(sop, next_op, out_flow.target.port);
    }
}

}  // namespace optimize
