#ifndef UTILS_PRINTDAG_H
#define UTILS_PRINTDAG_H

#include <graphviz/gvc.h>

#include "dag/DAG.h"
#include "dag/DAGOperator.h"

Agnode_t *buildDOT(DAGOperator *op, Agraph_t *g);
void printDAG(DAG *dag);

#endif  // UTILS_PRINTDAG_H
