#include "materialize_multiple_reads.hpp"

#include <vector>

#include <boost/range/algorithm.hpp>

#include "dag/dag.hpp"
#include "dag/operators/all_operator_declarations.hpp"
#include "dag/operators/all_operator_definitions.hpp"
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

    [[nodiscard]] auto operators() const { return operators_; }

private:
    const DAG *const dag_;
    std::vector<DAGOperator *> operators_;
};

namespace optimize {

void MaterializeMultipleReads::Run(DAG *const dag,
                                   const std::string & /*config*/) const {
    // Collect all operators with several consumers
    CollectMultipleReadsVisitor operator_collector(dag);
    for (auto const op : dag->operators()) {
        operator_collector.Visit(op);
    }

    for (auto const &op : operator_collector.operators()) {
        // Collect out flows
        std::vector<DAG::Flow> out_flows;
        boost::copy(dag->out_flows(op), std::back_inserter(out_flows));

        // Add materialization operator
        auto mat_op = new DAGMaterializeRowVector();
        dag->AddOperator(mat_op);
        dag->AddFlow(op, mat_op);

        // Replace out flows with scans
        for (auto const &out_flow : out_flows) {
            assert(out_flow.source.port == 0);

            dag->RemoveFlow(out_flow);

            auto scan_op = new DAGRowScan();
            dag->AddOperator(scan_op);
            dag->AddFlow(mat_op, scan_op);
            dag->AddFlow(scan_op, out_flow.target.op, out_flow.target.port);
        }
    }
}

}  // namespace optimize
