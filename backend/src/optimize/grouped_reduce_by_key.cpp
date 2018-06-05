#include "grouped_reduce_by_key.hpp"

#include "dag/DAGOperators.h"
#include "utils/DAGVisitor.h"

class CollectReduceByKeyVisitor : public DAGVisitor {
public:
    explicit CollectReduceByKeyVisitor(DAG *const dag) : DAGVisitor(dag) {}
    void visit(DAGReduceByKey *op) override {
        auto const pred = dag()->predecessor(op);
        if (pred->fields[0].properties->count(FL_GROUPED) > 0 ||
            pred->fields[0].properties->count(FL_UNIQUE) > 0 ||
            pred->fields[0].properties->count(FL_SORTED) > 0) {
            reduce_by_keys_.emplace_back(op);
        }
    }
    std::vector<DAGReduceByKey *> reduce_by_keys_;
};

void GroupedReduceByKey::optimize() {
    CollectReduceByKeyVisitor visitor(dag_);
    visitor.StartVisit();

    for (auto const op : visitor.reduce_by_keys_) {
        std::unique_ptr<DAGReduceByKeyGrouped> new_op_ptr(
                new DAGReduceByKeyGrouped());
        auto const new_op = new_op_ptr.get();

        new_op->fields = op->fields;
        new_op->llvm_ir = op->llvm_ir;
        new_op->output_type = op->output_type;

        dag_->AddOperator(new_op_ptr.release());

        if (op == dag_->sink) {
            dag_->sink = new_op;
        } else {
            const auto out_flow = dag_->out_flow(op);
            dag_->RemoveFlow(out_flow);
            dag_->AddFlow(new_op, 0, out_flow.target, out_flow.target_port);
        }

        const auto in_flow = dag_->in_flow(op);
        dag_->RemoveFlow(in_flow);
        dag_->AddFlow(in_flow.source, in_flow.source_port, new_op, 0);

        dag_->RemoveOperator(op);
    }
}
