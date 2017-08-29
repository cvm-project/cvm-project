
#include "utils/DAGVisitor.h"

void DAGCollection::accept(DAGVisitor &v) {
    v.visit(this);
}
