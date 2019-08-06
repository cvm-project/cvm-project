#include "assert_correct_open_next_close.hpp"

#include <vector>

#include <boost/mpl/list.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "dag/dag.hpp"
#include "dag/operators/assert_correct_open_next_close.hpp"
#include "dag/operators/operator.hpp"
#include "dag/operators/pipeline.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/visitor.hpp"

void AddAssertions(DAG *const dag) {
    std::vector<DAGOperator *> operators;
    boost::copy(dag->operators(), std::back_inserter(operators));

    for (auto const op : operators) {
        std::vector<DAG::Flow> out_flows;
        boost::copy(dag->out_flows(op), std::back_inserter(out_flows));

        auto const aop = new DAGAssertCorrectOpenNextClose();
        dag->AddOperator(aop);

        for (auto const &f : out_flows) {
            dag->RemoveFlow(f);
            dag->AddFlow(aop, f.source.port, f.target.op, f.target.port);
        }

        assert(op->num_out_ports() == 1);
        dag->AddFlow(op, aop);

        if (dag->output().op == op) dag->set_output(aop, dag->output().port);
    }
}

class AddAssertionsVisitor
    : public Visitor<AddAssertionsVisitor, const DAGOperator,
                     boost::mpl::list<    //
                             DAGPipeline  //
                             >::type> {
public:
    explicit AddAssertionsVisitor(DAG *const dag) : dag_(dag) {}

    void operator()(const DAGPipeline *const op) {
        AddAssertions(dag_->inner_dag(op));
    }

private:
    DAG *const dag_;
};

namespace optimize {

void AssertCorrectOpenNextClose::Run(DAG *const dag,
                                     const std::string & /*config*/) const {
    dag::utils::ApplyInReverseTopologicalOrderRecursively(
            dag, [](const DAGOperator *const op, DAG *const dag) {
                AddAssertionsVisitor(dag).Visit(op);
            });
}

}  // namespace optimize
