#include "verify.hpp"

#include <stdexcept>

#include <boost/format.hpp>

#include "dag/dag.hpp"
#include "dag/operators/operator.hpp"

void VerifyDag(const DAG *dag);
void VerifyOperator(const DAG *dag, const DAGOperator *op);

void VerifyDag(const DAG *const dag) {
    if (dag->num_out_ports() == 0) {
        throw std::runtime_error(
                (boost::format("DAG %1% does not have any outputs.\n") % dag)
                        .str());
    }

    // Verify operators
    for (auto *const op : dag->operators()) {
        VerifyOperator(dag, op);
    }
}

void VerifyOperator(const DAG *const dag, const DAGOperator *const op) {
    // Each input port must have a predecessor
    if (dag->in_degree(op) != op->num_in_ports()) {
        throw std::runtime_error(
                (boost::format(
                         "Operator %1% (%2%) has the wrong number of inputs:\n"
                         "  Number of in_flows: %3%\n"
                         "  Number of input ports: %4%\n") %
                 op->id % op->name() % dag->in_degree(op) % op->num_in_ports())
                        .str());
    }

    // Each output port must have at least one successor (except if the operator
    // is the output of the DAG)
    if (boost::distance(dag->outputs(op)) == 0) {
        size_t total_num_out_flows = 0;
        for (size_t i = 0; i < op->num_out_ports(); i++) {
            auto const num_out_flows = boost::distance(dag->out_flows(op, i));
            total_num_out_flows += num_out_flows;
            if (num_out_flows == 0) {
                throw std::runtime_error(
                        (boost::format("Operator %1% (%2%) does not have any "
                                       "successor on port %3%.\n") %
                         op->id % op->name() % i)
                                .str());
            }
        }

        // If the number of out flows on valid ports doesn't sum up to the out
        // degree (=total number of out flows), then there must be out flows
        // with higher port, invalid numbers.
        if (total_num_out_flows != dag->out_degree(op)) {
            throw std::runtime_error(
                    (boost::format("Operator %1% (%2%) has an out flow on an "
                                   "invalid port.\n") %
                     op->id % op->name())
                            .str());
        }
    }

    // Verify inner DAG
    if (dag->has_inner_dag(op)) {
        auto *const inner_dag = dag->inner_dag(op);

        // The number of input ports of the DAG must match the number of input
        // ports of the operator
        if (inner_dag->num_in_ports() != op->num_in_ports()) {
            throw std::runtime_error(
                    (boost::format("Operator %1% (%2%) has an inner DAG with a "
                                   "non-matching number of input ports:\n"
                                   "  Operator number of input ports: %3%\n"
                                   "  DAG number of input ports: %4%\n") %
                     op->id % op->name() % op->num_in_ports() %
                     inner_dag->num_in_ports())
                            .str());
        }

        // The number of output ports of the DAG must match the number of output
        // ports of the operator
        if (inner_dag->num_out_ports() != op->num_out_ports()) {
            throw std::runtime_error(
                    (boost::format("Operator %1% (%2%) has an inner DAG with a "
                                   "non-matching number of output ports:\n"
                                   "  Operator number of output ports: %3%\n"
                                   "  DAG number of output ports: %4%\n") %
                     op->id % op->name() % op->num_out_ports() %
                     inner_dag->num_out_ports())
                            .str());
        }

        // Recurse into inner DAG
        VerifyDag(inner_dag);
    }
}

namespace optimize {

void Verify::Run(DAG *const dag, const std::string & /*config*/) const {
    VerifyDag(dag);
}

}  // namespace optimize
