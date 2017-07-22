
#include "DAGRange.h"
#include "utils/DAGVisitor.h"

void DAGRange::accept(DAGVisitor &v) {
    v.visit(this);
}