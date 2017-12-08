
#include "DAGCartesian.h"
#include "utils/DAGVisitor.h"

void DAGCartesian::accept(DAGVisitor &v) { v.visit(this); }