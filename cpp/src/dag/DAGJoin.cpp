
#include "DAGJoin.h"
#include "utils/DAGVisitor.h"

void DAGJoin::accept(DAGVisitor *v) { v->visit(this); }
