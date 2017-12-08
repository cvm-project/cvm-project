
#include "DAGReduce.h"
#include "utils/DAGVisitor.h"

void DAGReduce::accept(DAGVisitor &v) {
    v.visit(this);
}