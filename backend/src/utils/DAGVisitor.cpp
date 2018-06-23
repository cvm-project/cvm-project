#include "DAGVisitor.h"

#include "dag/utils/apply_visitor.hpp"

void DAGVisitor::StartVisit() {
    dag::utils::ApplyInReverseTopologicalOrder(
            dag_, [&](auto op) { op->accept(this); });
}
