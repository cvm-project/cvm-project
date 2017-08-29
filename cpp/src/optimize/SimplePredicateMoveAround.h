//
// Created by sabir on 29.08.17.
//

#ifndef CPP_SIMPLEPREDICATEMOVEAROUND_H
#define CPP_SIMPLEPREDICATEMOVEAROUND_H


#include <dag/DAG.h>
#include <utils/DAGVisitor.h>

class SimplePredicateMoveAround : DAGVisitor {
public:
    void optimize(DAG *);

    void visit(DAGFilter *op);

private:
    vector<DAGFilter *> filters;
    DAG *dag;
};


#endif //CPP_SIMPLEPREDICATEMOVEAROUND_H
