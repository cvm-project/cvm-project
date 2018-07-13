#include "grouped_reduce_by_key.hpp"

#include <boost/mpl/list.hpp>

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

void GroupedReduceByKey::optimize() {
    CollectReduceByKeyVisitor visitor(dag_);
    for (auto const op : dag_->operators()) {
        visitor.Visit(op);
    }

    for (auto const op : visitor.reduce_by_keys_) {
        std::unique_ptr<DAGReduceByKeyGrouped> new_op_ptr(
                new DAGReduceByKeyGrouped());
        auto const new_op = new_op_ptr.get();

        new_op->tuple = std::make_unique<dag::collection::Tuple>(*op->tuple);
        new_op->llvm_ir = op->llvm_ir;

        dag_->AddOperator(new_op_ptr.release());

        const auto out_flow = dag_->out_flow(op);
        const auto in_flow = dag_->in_flow(op);

        dag_->RemoveFlow(out_flow);
        dag_->RemoveFlow(in_flow);

        dag_->AddFlow(new_op, 0, out_flow.target.op, out_flow.target.port);
        dag_->AddFlow(in_flow.source.op, in_flow.source.port, new_op, 0);

        dag_->RemoveOperator(op);
    }
}
