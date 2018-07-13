#include "materialize_multiple_reads.hpp"

#include <vector>

#include <boost/range/algorithm.hpp>

#include "dag/DAGOperators.h"
#include "dag/all_operator_declarations.hpp"
#include "utils/visitor.hpp"

class CollectMultipleReadsVisitor
    : public Visitor<CollectMultipleReadsVisitor, DAGOperator,
                     dag::AllOperatorTypes> {
public:
    explicit CollectMultipleReadsVisitor(const DAG *const dag) : dag_(dag) {}

    void operator()(DAGMaterializeRowVector *const /*op*/) {}
    void operator()(DAGOperator *const op) {
        if (dag_->out_degree(op) > 1) operators_.emplace_back(op);
    }

    std::vector<DAGOperator *> operators_;

private:
    const DAG *const dag_;
};

void MaterializeMultipleReads::optimize() {
    // Collect all operators with several consumers
    CollectMultipleReadsVisitor operator_collector(dag_);
    for (auto const op : dag_->operators()) {
        operator_collector.Visit(op);
    }

    for (auto const &op : operator_collector.operators_) {
        // Collect out flows
        std::vector<DAG::Flow> out_flows;
        boost::copy(dag_->out_flows(op), std::back_inserter(out_flows));

        // Add materialization operator
        auto mat_op = new DAGMaterializeRowVector();
        dag_->AddOperator(mat_op);
        dag_->AddFlow(op, mat_op);

        // Replace out flows with scans
        for (auto const &out_flow : out_flows) {
            assert(out_flow.source.port == 0);

            dag_->RemoveFlow(out_flow);

            auto scan_op = new DAGCollection();
            dag_->AddOperator(scan_op);
            dag_->AddFlow(mat_op, scan_op);
            dag_->AddFlow(scan_op, out_flow.target.op, out_flow.target.port);
        }
    }
}
