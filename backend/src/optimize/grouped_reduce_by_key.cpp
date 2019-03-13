#include "grouped_reduce_by_key.hpp"

#include <boost/mpl/list.hpp>

#include <jbcoe/polymorphic_value.h>

#include "dag/DAG.h"
#include "dag/DAGOperators.h"
#include "utils/visitor.hpp"

struct CollectReduceByKeyVisitor
    : public Visitor<CollectReduceByKeyVisitor, DAGOperator,
                     boost::mpl::list<DAGReduceByKey>> {
    explicit CollectReduceByKeyVisitor(const DAG *const dag) : dag_(dag) {}
    void operator()(DAGReduceByKey *op) {
        auto const pred = dag_->predecessor(op);
        if (pred->tuple->fields[0]->properties().count(
                    dag::collection::FL_GROUPED) > 0) {
            reduce_by_keys_.emplace_back(op);
        }
    }
    std::vector<DAGReduceByKey *> reduce_by_keys_;
    const DAG *const dag_;
};

namespace optimize {

void GroupedReduceByKey::Run(DAG *const dag) const {
    CollectReduceByKeyVisitor visitor(dag);
    for (auto const op : dag->operators()) {
        visitor.Visit(op);
    }

    for (auto const op : visitor.reduce_by_keys_) {
        std::unique_ptr<DAGReduceByKeyGrouped> new_op_ptr(
                new DAGReduceByKeyGrouped());
        auto const new_op = new_op_ptr.get();

        new_op->tuple = jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                *op->tuple);
        new_op->llvm_ir = op->llvm_ir;

        dag->AddOperator(new_op_ptr.release());

        const auto out_flow = dag->out_flow(op);
        const auto in_flow = dag->in_flow(op);

        dag->RemoveFlow(out_flow);
        dag->RemoveFlow(in_flow);

        dag->AddFlow(new_op, 0, out_flow.target.op, out_flow.target.port);
        dag->AddFlow(in_flow.source.op, in_flow.source.port, new_op, 0);

        dag->RemoveOperator(op);
    }
}

}  // namespace optimize
