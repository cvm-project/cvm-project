#include "DAGVisitor.h"

#include "dag/DAG.h"
#include "dag/DAGOperator.h"
#include "utils/debug_print.h"

void visit_impl(DAGVisitor *const v, DAGOperator *const op) {
    DEBUG_PRINT(("Visiting " + std::to_string(op->id) + " (" + op->name() +
                 ")").c_str());
    for (auto &p : op->predecessors) {
        visit_impl(v, p);
    }
    op->accept(v);
}

void DAGVisitor::visitDag(DAG *const dag) { visit_impl(this, dag->sink); }
