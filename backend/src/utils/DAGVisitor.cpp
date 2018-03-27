#include "DAGVisitor.h"

#include "dag/DAG.h"
#include "dag/DAGOperator.h"
#include "utils/debug_print.h"

void visit_impl(const DAG *const dag, DAGVisitor *const v,
                DAGOperator *const op) {
    DEBUG_PRINT(("Visiting " + std::to_string(op->id) + " (" + op->name() + ")")
                        .c_str());
    for (const auto &f : dag->in_flows(op)) {
        visit_impl(dag, v, f.source);
    }
    op->accept(v);
}

void DAGVisitor::StartVisit() { visit_impl(dag_, this, dag_->sink); }
