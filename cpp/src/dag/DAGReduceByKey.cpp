
#include "DAGReduceByKey.h"
#include "utils/DAGVisitor.h"

void DAGReduceByKey::accept(DAGVisitor &v) {
    v.visit(this);
}