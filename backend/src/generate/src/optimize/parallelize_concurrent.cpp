#include "parallelize_concurrent.hpp"

#include <vector>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/iterator_range.hpp>
#include <polymorphic_value.h>

#include "dag/dag.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/type/atomic.hpp"
#include "dag/type/tuple.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "dag/utils/type_traits.hpp"
#include "optimize/utils.hpp"

using dag::utils::IsInstanceOf;

namespace optimize {

auto IsParallizationSeed(DAGOperator *const op) -> bool {
    return IsInstanceOf<            //
            DAGAntiJoin,            //
            DAGAntiJoinPredicated,  //
            DAGCartesian,           //
            DAGExpandPattern,       //
            DAGJoin,                //
            DAGSemiJoin,            //
            DAGRange,               //
            DAGReduce,              //
            DAGReduceByKey          //
            >(op);
}

class HandleSeedOperatorVisitor
    : public Visitor<HandleSeedOperatorVisitor, DAGOperator,
                     boost::mpl::list<               //
                             DAGAntiJoin,            //
                             DAGAntiJoinPredicated,  //
                             DAGCartesian,           //
                             DAGExpandPattern,       //
                             DAGJoin,                //
                             DAGSemiJoin,            //
                             DAGRange,               //
                             DAGReduce,              //
                             DAGReduceByKey          //
                             >::type,
                     DAGConcurrentExecute *> {
public:
    // cppcheck-suppress noExplicitConstructor  // false positive
    explicit HandleSeedOperatorVisitor(DAG *const dag) : dag_(dag) {}

    auto operator()(DAGCartesian *const op) -> DAGConcurrentExecute * {
        auto const left_pred_op = dag_->predecessor(op, 0);
        auto const right_pred_op = dag_->predecessor(op, 1);
        auto const next_op = dag_->successor(op);
        auto const left_in_flow = dag_->in_flow(op, 0);
        auto const right_in_flow = dag_->in_flow(op, 1);
        auto const out_flow = dag_->out_flow(op);

        // Create parallelize operator
        auto const pop = new DAGConcurrentExecute();
        dag_->AddOperator(pop);
        dag_->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag_->inner_dag(pop);
        pop->num_inputs = 2;

        // Fill the parallelize operator with parameter lookups
        auto const left_param_op = new DAGParameterLookup();
        inner_dag->AddOperator(left_param_op);
        inner_dag->set_input(0, left_param_op);

        auto const right_param_op = new DAGParameterLookup();
        inner_dag->AddOperator(right_param_op);
        inner_dag->set_input(1, right_param_op);

        // Broadcast the right side
        auto const bc_op = new DAGBroadcast();
        inner_dag->AddOperator(bc_op);
        inner_dag->AddFlow(right_param_op, bc_op);

        // Move seed operator into parallelize operator
        dag_->RemoveFlow(left_in_flow);
        dag_->RemoveFlow(right_in_flow);
        dag_->RemoveFlow(out_flow);
        dag_->MoveOperator(inner_dag, op);
        inner_dag->AddFlow(left_param_op, op, 0);
        inner_dag->AddFlow(bc_op, op, 1);
        inner_dag->set_output(op);

        // Connect parallelize operator with old successor and predecessors
        dag_->AddFlow(left_pred_op, pop, 0);
        dag_->AddFlow(right_pred_op, pop, 1);
        dag_->AddFlow(pop, next_op, out_flow.target.port);

        return pop;
    }

    auto operator()(DAGExpandPattern *const op) -> DAGConcurrentExecute * {
        auto const pattern_pred_op = dag_->predecessor(op, 0);
        auto const data_pred_op = dag_->predecessor(op, 1);
        auto const next_op = dag_->successor(op);
        auto const pattern_in_flow = dag_->in_flow(op, 0);
        auto const data_in_flow = dag_->in_flow(op, 1);
        auto const out_flow = dag_->out_flow(op);

        // Create parallelize operator
        auto const pop = new DAGConcurrentExecute();
        dag_->AddOperator(pop);
        dag_->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag_->inner_dag(pop);
        pop->num_inputs = 2;

        // Fill the parallelize operator with parameter lookups
        auto const pattern_param_op = new DAGParameterLookup();
        inner_dag->AddOperator(pattern_param_op);
        inner_dag->set_input(0, pattern_param_op);

        auto const data_param_op = new DAGParameterLookup();
        inner_dag->AddOperator(data_param_op);
        inner_dag->set_input(1, data_param_op);

        // Broadcast the pattern side
        auto const bc_op = new DAGBroadcast();
        inner_dag->AddOperator(bc_op);
        inner_dag->AddFlow(pattern_param_op, bc_op);

        // Move seed operator into parallelize operator
        dag_->RemoveFlow(pattern_in_flow);
        dag_->RemoveFlow(data_in_flow);
        dag_->RemoveFlow(out_flow);
        dag_->MoveOperator(inner_dag, op);
        inner_dag->AddFlow(bc_op, op, 0);
        inner_dag->AddFlow(data_param_op, op, 1);
        inner_dag->set_output(op);

        // Connect parallelize operator with old successor and predecessors
        dag_->AddFlow(pattern_pred_op, pop, 0);
        dag_->AddFlow(data_pred_op, pop, 1);
        dag_->AddFlow(pop, next_op, out_flow.target.port);

        return pop;
    }

private:
    auto HandleJoin(DAGOperator *const op) -> DAGConcurrentExecute * {
        auto const left_pred_op = dag_->predecessor(op, 0);
        auto const right_pred_op = dag_->predecessor(op, 1);
        auto const next_op = dag_->successor(op);
        auto const left_in_flow = dag_->in_flow(op, 0);
        auto const right_in_flow = dag_->in_flow(op, 1);
        auto const out_flow = dag_->out_flow(op);

        // Create parallelize operator
        auto const pop = new DAGConcurrentExecute();
        dag_->AddOperator(pop);
        dag_->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag_->inner_dag(pop);
        pop->num_inputs = 2;

        // Fill the parallelize operator with parameter lookups
        auto const left_param_op = new DAGParameterLookup();
        inner_dag->AddOperator(left_param_op);
        inner_dag->set_input(0, left_param_op);

        auto const right_param_op = new DAGParameterLookup();
        inner_dag->AddOperator(right_param_op);
        inner_dag->set_input(1, right_param_op);

        // Add exchange operators
        auto const left_exchange_op = new DAGExchange();
        inner_dag->AddOperator(left_exchange_op);
        inner_dag->AddFlow(left_param_op, left_exchange_op);

        auto const right_exchange_op = new DAGExchange();
        inner_dag->AddOperator(right_exchange_op);
        inner_dag->AddFlow(right_param_op, right_exchange_op);

        // Move seed operator into parallelize operator
        dag_->RemoveFlow(left_in_flow);
        dag_->RemoveFlow(right_in_flow);
        dag_->RemoveFlow(out_flow);
        dag_->MoveOperator(inner_dag, op);
        inner_dag->AddFlow(left_exchange_op, op, 0);
        inner_dag->AddFlow(right_exchange_op, op, 1);
        inner_dag->set_output(op);

        // Connect parallelize operator with old successor and predecessors
        dag_->AddFlow(left_pred_op, pop, 0);
        dag_->AddFlow(right_pred_op, pop, 1);
        dag_->AddFlow(pop, next_op, out_flow.target.port);

        return pop;
    }

public:
    auto operator()(DAGAntiJoin *const op) -> DAGConcurrentExecute * {
        return HandleJoin(op);
    }

    auto operator()(DAGAntiJoinPredicated *const op) -> DAGConcurrentExecute * {
        return HandleJoin(op);
    }

    auto operator()(DAGJoin *const op) -> DAGConcurrentExecute * {
        return HandleJoin(op);
    }

    auto operator()(DAGSemiJoin *const op) -> DAGConcurrentExecute * {
        return HandleJoin(op);
    }

    auto operator()(DAGRange *const op) -> DAGConcurrentExecute * {
        auto const in_flow = dag_->in_flow(op);
        auto const out_flow = dag_->out_flow(op);

        // Create parallelize operator
        auto const pop = new DAGConcurrentExecute();
        dag_->AddOperator(pop);
        dag_->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag_->inner_dag(pop);

        // Create split operator
        auto const split_op = MakeSplitOperator(op);
        dag_->AddOperator(split_op);

        // Create degree-of-parallelism operator
        auto const dop_op = new DAGConstantTuple();
        dag_->AddOperator(dop_op);
        dop_op->values.emplace_back("$DOP");
        dop_op->tuple = jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                dag::type::Tuple::MakeTuple(
                        {dag::type::Atomic::MakeAtomic("long")}));

        // Fix up outer DAG
        dag_->RemoveFlow(in_flow);
        dag_->RemoveFlow(out_flow);
        dag_->AddFlow(in_flow.source, split_op, 0);
        dag_->AddFlow(dop_op, split_op, 1);
        dag_->AddFlow(split_op, pop);
        dag_->AddFlow(pop, out_flow.target);

        // Populate parallel inner DAG
        auto const param_op = new DAGParameterLookup();
        inner_dag->AddOperator(param_op);
        inner_dag->set_input(param_op);

        dag_->MoveOperator(inner_dag, op);
        inner_dag->AddFlow(param_op, op);
        inner_dag->set_output(op);

        return pop;
    }

    auto operator()(DAGReduce *const op) -> DAGConcurrentExecute * {
        auto const pred_op = dag_->predecessor(op, 0);
        auto const in_flow = dag_->in_flow(op, 0);

        // Create parallelize operator
        auto const pop = new DAGConcurrentExecute();
        dag_->AddOperator(pop);
        dag_->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag_->inner_dag(pop);

        // Populate parallel inner DAG
        auto const param_op = new DAGParameterLookup();
        inner_dag->AddOperator(param_op);
        inner_dag->set_input(param_op);

        auto pre_reduction_op_ptr = std::make_unique<DAGReduce>();
        auto const pre_reduction_op = pre_reduction_op_ptr.get();
        pre_reduction_op->llvm_ir = op->llvm_ir;

        auto const ensure_single_tuple_op = new DAGEnsureSingleTuple();
        inner_dag->AddOperator(ensure_single_tuple_op);

        inner_dag->AddOperator(pre_reduction_op_ptr.release());
        inner_dag->AddFlow(param_op, pre_reduction_op);
        inner_dag->AddFlow(pre_reduction_op, ensure_single_tuple_op);
        inner_dag->set_output(ensure_single_tuple_op);

        // Fix up outer DAG
        dag_->RemoveFlow(in_flow);
        dag_->AddFlow(pred_op, pop);
        dag_->AddFlow(pop, op);

        return pop;
    }

    auto operator()(DAGReduceByKey *const op) -> DAGConcurrentExecute * {
        auto const pred_op = dag_->predecessor(op, 0);
        auto const in_flow = dag_->in_flow(op);
        auto const out_flow = dag_->out_flow(op);

        // Create parallelize operator
        auto const pop = new DAGConcurrentExecute();
        dag_->AddOperator(pop);
        dag_->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag_->inner_dag(pop);

        // Move seed operator into parallelize operator
        dag_->RemoveFlow(in_flow);
        dag_->RemoveFlow(out_flow);
        dag_->MoveOperator(inner_dag, op);
        dag_->AddFlow(pred_op, pop);
        dag_->AddFlow(pop, out_flow.target);

        // Populate parallel inner DAG
        auto const param_op = new DAGParameterLookup();
        inner_dag->AddOperator(param_op);
        inner_dag->set_input(param_op);
        inner_dag->AddFlow(param_op, op);

        auto const exchange_op = new DAGExchange();
        inner_dag->AddOperator(exchange_op);
        inner_dag->AddFlow(op, exchange_op);

        auto post_red_op_ptr = std::make_unique<DAGReduceByKey>();
        auto const post_red_op = post_red_op_ptr.get();
        post_red_op->llvm_ir = op->llvm_ir;
        inner_dag->AddOperator(post_red_op_ptr.release());
        inner_dag->AddFlow(exchange_op, post_red_op);
        inner_dag->set_output(post_red_op);

        return pop;
    }

    DAG *const dag_;
};

void ExpandConcurrentExecutor(DAG *const dag, DAGConcurrentExecute *const pop) {
    while (true) {
        if (dag->out_degree(pop) > 1) {
            // Remember out flows...
            std::vector<DAG::Flow> in_flows;
            boost::copy(dag->out_flows(pop), std::back_inserter(in_flows));

            // ... and remove them
            for (auto const in_flow : in_flows) {
                dag->RemoveFlow(in_flow);
            }

            std::vector<DAGConcurrentExecute *> pops;

            // Move each successor into its concurrent executor
            for (auto const in_flow : in_flows) {
                auto const next_op = in_flow.target.op;
                auto const out_flow = dag->out_flow(next_op);

                dag->RemoveFlow(out_flow);

                // Create new concurrent executor
                auto const pop = new DAGConcurrentExecute();
                dag->AddOperator(pop);
                dag->set_inner_dag(pop, new DAG());
                auto const inner_dag = dag->inner_dag(pop);
                pops.push_back(pop);

                // Move operator
                dag->MoveOperator(inner_dag, next_op);

                // Populate inner DAG
                auto const param_op = new DAGParameterLookup();
                inner_dag->AddOperator(param_op);
                inner_dag->AddFlow(param_op, next_op);

                inner_dag->set_input(param_op);
                inner_dag->set_output(next_op);

                // Fix up outer DAG
                dag->AddFlow(in_flow.source, pop);
                dag->AddFlow(pop, out_flow.target);
            }

            for (auto const op : pops) {
                ExpandConcurrentExecutor(dag, op);
            }

            break;
        }

        auto const successor = dag->successor(pop);
        auto const inner_dag = dag->inner_dag(pop);

        if (IsInstanceOf<DAGFilter,                //
                         DAGColumnScan,            //
                         DAGMap,                   //
                         DAGMaterializeRowVector,  //
                         DAGParquetScan>(successor)) {
            if (boost::distance(dag->outputs(successor)) != 0) {
                break;
            }

            if (dag->out_degree(successor) == 1 &&
                IsInstanceOf<DAGEnsureSingleTuple>(dag->successor(successor))) {
                break;
            }

            // Remove in flow
            auto const in_flow = dag->in_flow(successor);
            dag->RemoveFlow(in_flow);

            // Remember out flows
            std::vector<DAG::Flow> out_flows;
            boost::copy(dag->out_flows(successor),
                        std::back_inserter(out_flows));

            // Remove out flows
            for (auto const out_flow : out_flows) {
                dag->RemoveFlow(out_flow);
                assert(!IsInstanceOf<DAGEnsureSingleTuple>(out_flow.target.op));
            }

            // Move operator
            dag->MoveOperator(inner_dag, successor);

            // Recreate out flows
            for (auto const out_flow : out_flows) {
                auto const next_op = out_flow.target.op;
                dag->AddFlow(pop, next_op, out_flow.target.port);
            }

            // Fix up outer DAG
            auto const old_output = inner_dag->output();
            inner_dag->AddFlow(old_output, successor);
            inner_dag->set_output(successor);

            continue;
        }

        break;
    }
}

struct CollectParallelOpsVisitor
    : public Visitor<CollectParallelOpsVisitor, DAGOperator,
                     boost::mpl::list<DAGConcurrentExecute>> {
    void operator()(DAGConcurrentExecute *const op) { pops_.push_back(op); }
    std::deque<DAGConcurrentExecute *> pops_;
};

void MergeParallelOperators(DAG *const dag) {
    CollectParallelOpsVisitor pops_collector;
    dag::utils::ApplyInTopologicalOrder(dag, pops_collector.functor());

    auto &pops = pops_collector.pops_;
    while (!pops.empty()) {
        auto const pop = pops.front();
        pops.pop_front();

        // Operator may have been merged already; skip it in that case
        if (!dag->contains(pop)) continue;

        auto const inner_dag = dag->inner_dag(pop);
        while (true) {
            // Find preceeding parallel operator
            auto const in_flow_range = dag->in_flows(pop);
            auto const in_flow_it =
                    boost::find_if(in_flow_range, [](auto const f) {
                        return IsInstanceOf<DAGConcurrentExecute>(f.source.op);
                    });

            if (in_flow_it == boost::end(in_flow_range)) break;

            auto const other_pop =
                    dynamic_cast<DAGConcurrentExecute *>(in_flow_it->source.op);
            auto const other_inner_dag = dag->inner_dag(other_pop);

            // Remember out flows of predecessor
            std::vector<DAG::Flow> inbetween_flows;
            std::vector<DAG::Flow> other_out_flows;

            for (auto const &f : dag->out_flows(other_pop)) {
                if (f.target.op == pop) {
                    inbetween_flows.push_back(f);
                } else {
                    other_out_flows.push_back(f);
                }
            }

            // Make sure we don't create a cycle: If we have
            // (other_pop)->(pop)->(X), we can't merge other_pop and X, as we
            //      `---------------^
            // would otherwise get (other_pop+X)<->(pop). Note that merging pop
            // and X would be fine.  We thus only merge if none of the
            // predecessors of other_pop is reachable from pop. To determine
            // reachability, we use the topologial order:
            // A < B => A not reachable from B.
            {
                std::unordered_map<const DAGOperator *, size_t> top_order;
                bool do_break = false;

                for (auto const &f : other_out_flows) {
                    // Determine topological order
                    if (top_order.empty()) {
                        dag::utils::ApplyInTopologicalOrder(
                                dag, [&](DAGOperator *const op) {
                                    top_order.emplace(op, top_order.size());
                                });
                    }

                    // If we would create a cycle, save pop to queue and come
                    // back later
                    if (top_order.at(pop) < top_order.at(f.target.op)) {
                        pops.push_back(pop);
                        do_break = true;
                        break;
                    }
                }

                // Stop expanding this pop for now
                if (do_break) break;
            }

            // Remember inputs, outputs, and flows of other DAG and delete them
            std::vector<DAGOperator *> temp_ops;
            std::vector<DAG::Flow> temp_flows;
            std::vector<std::pair<int, DAG::FlowTip>> temp_inputs;
            std::unordered_map<int, DAG::FlowTip> temp_outputs;

            boost::copy(other_inner_dag->operators(),
                        std::back_inserter(temp_ops));
            boost::copy(other_inner_dag->flows(),
                        std::back_inserter(temp_flows));
            boost::copy(other_inner_dag->inputs(),
                        std::back_inserter(temp_inputs));
            boost::copy(other_inner_dag->outputs(),
                        std::inserter(temp_outputs, temp_outputs.begin()));

            other_inner_dag->reset_inputs();
            other_inner_dag->reset_outputs();
            for (auto const f : temp_flows) {
                other_inner_dag->RemoveFlow(f);
            }

            // Move operators
            for (auto const op : temp_ops) {
                other_inner_dag->MoveOperator(inner_dag, op);
            }

            // Reconstruct flows
            for (auto const &f : temp_flows) {
                inner_dag->AddFlow(f.source.op, f.source.port, f.target.op,
                                   f.target.port);
            }

            // Connect old outputs of other DAG
            std::set<int> input_ports;
            for (auto const &in_flow : inbetween_flows) {
                auto const input_port = in_flow.target.port;
                auto const input_tip = inner_dag->input(input_port);
                auto const inner_flow = inner_dag->out_flow(input_tip.op);

                input_ports.insert(input_port);

                assert(IsInstanceOf<DAGParameterLookup>(input_tip.op));

                inner_dag->reset_input(input_port);
                inner_dag->RemoveFlow(inner_flow);
                inner_dag->RemoveOperator(input_tip.op);
                inner_dag->AddFlow(temp_outputs.at(in_flow.source.port),
                                   inner_flow.target);
            }
            pop->num_inputs += other_pop->num_inputs - inbetween_flows.size();

            // Connect old input of other DAG
            std::vector<DAG::Flow> in_flows;
            boost::copy(dag->in_flows(other_pop), std::back_inserter(in_flows));

            for (auto const &f : in_flows) {
                // Reuse old input port of target DAG in order to preserve
                // existing port numbering
                int new_port = inner_dag->in_degree();
                if (!input_ports.empty()) {
                    new_port = *(input_ports.begin());
                    input_ports.erase(input_ports.begin());
                }

                dag->RemoveFlow(f);
                dag->AddFlow(f.source, pop, new_port);

                auto const old_input = temp_inputs.at(f.target.port);
                assert(old_input.first == f.target.port);
                inner_dag->set_input(new_port, old_input.second.op);
            }

            // Remove flows between the two parallel operators and remove other
            // (which is now empty)
            for (auto const &f : inbetween_flows) {
                dag->RemoveFlow(f);
            }

            // Connect out flows that go to downstream operators
            for (auto const &f : other_out_flows) {
                // Remove old flow
                dag->RemoveFlow(f);

                // Connect output of new pop
                auto const output_tip = temp_outputs.at(f.source.port);
                auto const dag_output_port = inner_dag->num_out_ports();
                inner_dag->set_output(dag_output_port, output_tip);
                pop->num_outputs++;

                // Connect intput of old consumer
                dag->AddFlow(pop, dag_output_port, f.target);
            }

            dag->RemoveOperator(other_pop);
        }
    }
}

void ParallelizeConcurrent::Run(DAG *const dag,
                                const std::string & /*config*/) const {
    // Collect seed operators
    std::vector<DAGOperator *> seeds;
    for (auto const op : dag->operators()) {
        if (IsParallizationSeed(op)) seeds.push_back(op);
    }

    // Put seed operators into parallel operators
    HandleSeedOperatorVisitor visitor(dag);
    for (auto const op : seeds) {
        auto const pop = visitor.Visit(op);
        ExpandConcurrentExecutor(dag, pop);
    }

    MergeParallelOperators(dag);

    // Don't broadcast constants
    std::vector<DAGOperator *> constant_ops;
    for (auto const op : dag->operators()) {
        if (IsInstanceOf<DAGConstantTuple>(op)) constant_ops.push_back(op);
    }

    for (auto const op : constant_ops) {
        if (!IsInstanceOf<DAGConstantTuple>(op)) continue;

        auto const outer_flow = dag->out_flow(op);
        auto const pop_cand = outer_flow.target.op;

        if (!IsInstanceOf<DAGConcurrentExecute>(pop_cand)) continue;

        auto const pop = dynamic_cast<DAGConcurrentExecute *>(pop_cand);
        auto const inner_dag = dag->inner_dag(pop);

        auto const param_op = inner_dag->input(outer_flow.target.port).op;
        assert(IsInstanceOf<DAGParameterLookup>(param_op));

        auto const broadcast_op = inner_dag->successor(param_op);

        if (!IsInstanceOf<DAGBroadcast>(broadcast_op)) continue;

        // Fix up outer DAG
        dag->RemoveFlow(outer_flow);
        dag->MoveOperator(inner_dag, op);

        // Fix up inputs of inner DAG
        std::vector<std::pair<int, DAG::FlowTip>> temp_inputs;
        for (auto const &input : inner_dag->inputs()) {
            if (input.second.op != param_op) temp_inputs.emplace_back(input);
        }
        inner_dag->reset_inputs();
        for (auto const &input : temp_inputs) {
            inner_dag->set_input(input.first, input.second);
        }

        // Fix up pop
        pop->num_inputs--;

        // Fix up inner DAG
        auto const out_flow = inner_dag->out_flow(broadcast_op);
        inner_dag->RemoveFlow(out_flow);
        inner_dag->RemoveFlow(inner_dag->in_flow(broadcast_op));
        inner_dag->RemoveOperator(param_op);
        inner_dag->RemoveOperator(broadcast_op);
        inner_dag->AddFlow(op, out_flow.target);
    }
}

}  // namespace optimize
