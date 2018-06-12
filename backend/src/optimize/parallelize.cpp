#include "parallelize.hpp"

#include <unordered_set>

#include <boost/mpl/list.hpp>

#include "dag/DAG.h"
#include "dag/DAGOperators.h"
#include "dag/utils/type_traits.hpp"
#include "utils/visitor.hpp"

using dag::utils::IsInstanceOf;

bool IsSourceOperator(DAGOperator *const op) {
    return IsInstanceOf<    //
            DAGCollection,  //
            DAGRange        //
            >(op);
}

DAGOperator *MakeSplitOperator(const DAGOperator *const op) {
    class MakeSplitOperatorVisitor
        : public Visitor<MakeSplitOperatorVisitor, const DAGOperator,
                         boost::mpl::list<       //
                                 DAGCollection,  //
                                 DAGRange        //
                                 >::type,
                         DAGOperator *> {
    public:
        DAGOperator *operator()(const DAGCollection * /*op*/) {
            return new DAGSplitCollection();
        }
        DAGOperator *operator()(const DAGRange * /*op*/) {
            return new DAGSplitRange();
        }
    };
    return MakeSplitOperatorVisitor().Visit(op);
}

namespace optimize {

void Parallelize::Run(DAG *const dag) const {
    std::unordered_set<DAGParallelMap *> parallelize_operators;

    // Insert (parallelize) --> (sequentialize) operators before all source
    // operators
    for (const auto op : dag->operators()) {
        if (!IsSourceOperator(op)) continue;

        assert(dag->in_degree(op) == 1);
        assert(dag->out_degree(op) == 1);

        auto const pred_op = dag->predecessor(op);
        auto const next_op = dag->successor(op);
        auto const in_flow = dag->in_flow(op);
        auto const out_flow = dag->out_flow(op);

        // Create split operator
        auto const split_op = MakeSplitOperator(op);
        dag->AddOperator(split_op);

        // Create parallelize operator
        auto const pop = new DAGParallelMap();
        dag->AddOperator(pop);
        dag->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag->inner_dag(pop);

        // Fill the parallelize operator with a parameter lookup
        auto const param_op = new DAGParameterLookup();
        inner_dag->AddOperator(param_op);
        inner_dag->set_input(param_op);

        // Move source operator into parallelize operator
        dag->RemoveFlow(in_flow);
        dag->RemoveFlow(out_flow);
        dag->MoveOperator(inner_dag, op);
        inner_dag->AddFlow(param_op, op);
        inner_dag->set_output(op);

        // Connect parallelize operator with old successor and predecessor
        dag->AddFlow(pred_op, split_op);
        dag->AddFlow(split_op, in_flow.source.port, pop);
        dag->AddFlow(pop, next_op, out_flow.target.port);

        parallelize_operators.insert(pop);
    }

    // Extend parallelize operator
    for (const auto op : parallelize_operators) {
        auto const inner_dag = dag->inner_dag(op);
        do {
            if (dag->out_degree(op) != 1) break;
            if (IsInstanceOf<DAGFilter, DAGMap>(dag->successor(op))) {
                const auto other_op = dag->successor(op);
                const auto next_op = dag->successor(other_op);

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
                auto const red_op = dag->successor(op);

                // Pre-reduce operator
                auto const pre_reduction_op = new DAGReduce();
                inner_dag->AddOperator(pre_reduction_op);
                pre_reduction_op->llvm_ir = red_op->llvm_ir;

                // Flows
                inner_dag->AddFlow(inner_dag->output().op, pre_reduction_op);
                inner_dag->set_output(pre_reduction_op);

                break;
            }
            if (IsInstanceOf<DAGReduceByKey>(dag->successor(op)) ||
                IsInstanceOf<DAGReduceByKeyGrouped>(dag->successor(op))) {
                auto const other_op = dag->successor(op);

                DAGOperator *pre_reduction_op{};
                if (IsInstanceOf<DAGReduceByKey>(other_op)) {
                    auto const vpr =
                            inner_dag->AddOperator(new DAGReduceByKey());
                    pre_reduction_op = inner_dag->to_operator(vpr);
                } else {
                    assert(IsInstanceOf<DAGReduceByKeyGrouped>(other_op));
                    auto const vpr =
                            inner_dag->AddOperator(new DAGReduceByKeyGrouped());
                    pre_reduction_op = inner_dag->to_operator(vpr);
                }

                pre_reduction_op->llvm_ir = other_op->llvm_ir;

                inner_dag->AddFlow(inner_dag->output().op, pre_reduction_op);
                inner_dag->set_output(pre_reduction_op);

                break;
            }
            break;
        } while (true);

        if (IsInstanceOf<DAGReduce>(inner_dag->output().op)) return;

        // Add materialize operator at end of inner plan of parallel map
        auto const mat_op = new DAGMaterializeRowVector();
        inner_dag->AddOperator(mat_op);

        inner_dag->AddFlow(inner_dag->output().op, mat_op);
        inner_dag->set_output(mat_op);

        // Add scan operator after parallel map to get back individual tuples
        auto const next_op = dag->successor(op);
        auto const out_flow = dag->out_flow(op);

        auto const sop = new DAGCollection();
        dag->AddOperator(sop);

        dag->RemoveFlow(out_flow);
        dag->AddFlow(op, sop);
        dag->AddFlow(sop, next_op, out_flow.target.port);
    }
}

}  // namespace optimize
