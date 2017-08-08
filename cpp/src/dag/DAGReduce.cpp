
#include "utils/DAGVisitor.h"
#include "DAGReduce.h"

void DAGReduce::accept(DAGVisitor &v) {
    v.visit(this);
}