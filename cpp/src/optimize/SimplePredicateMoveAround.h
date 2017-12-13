//
// Created by sabir on 29.08.17.
//

#ifndef CPP_SIMPLEPREDICATEMOVEAROUND_H
#define CPP_SIMPLEPREDICATEMOVEAROUND_H

#include "dag/DAG.h"
#include "utils/DAGVisitor.h"

// cppcheck-suppress noConstructor
class SimplePredicateMoveAround : DAGVisitor {
public:
    void optimize(DAG *dag_);

    void visit(DAGFilter *op);

private:
    vector<DAGFilter *> filters;
    DAG *dag;
};

#endif  // CPP_SIMPLEPREDICATEMOVEAROUND_H
