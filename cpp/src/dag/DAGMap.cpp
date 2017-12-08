
#include "DAGMap.h"
#include "utils/DAGVisitor.h"

void DAGMap::accept(DAGVisitor &v) { v.visit(this); }