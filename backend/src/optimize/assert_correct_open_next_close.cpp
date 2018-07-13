#include "assert_correct_open_next_close.hpp"

#include <vector>

#include <boost/range/algorithm/copy.hpp>

#include "dag/DAGAssertCorrectOpenNextClose.h"
#include "dag/DAGOperator.h"

void AssertCorrectOpenNextClose::optimize() {
    std::vector<DAGOperator *> operators;
    boost::copy(dag_->operators(), std::back_inserter(operators));

    for (auto const op : operators) {
        std::vector<DAG::Flow> out_flows;
        boost::copy(dag_->out_flows(op), std::back_inserter(out_flows));

        auto const aop = new DAGAssertCorrectOpenNextClose();
        dag_->AddOperator(aop);

        for (auto const &f : out_flows) {
            dag_->RemoveFlow(f);
            dag_->AddFlow(aop, f.source_port, f.target, f.target_port);
        }

        assert(op->num_out_ports() == 1);
        dag_->AddFlow(op, aop);

        if (dag_->sink() == op) dag_->set_sink(aop);
    }
}
