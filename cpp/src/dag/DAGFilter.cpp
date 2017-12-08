
#include "utils/DAGVisitor.h"

void DAGFilter::accept(DAGVisitor &v) { v.visit(this); }