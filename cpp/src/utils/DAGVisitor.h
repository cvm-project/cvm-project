//
// Created by sabir on 21.07.17.
//

#ifndef UTILS_DAGVISITOR_H
#define UTILS_DAGVISITOR_H

#include "dag/DAGOperator.h"

class DAG;
class DAGCartesian;
class DAGCollection;
class DAGFilter;
class DAGJoin;
class DAGMap;
class DAGRange;
class DAGReduce;
class DAGReduceByKey;

class DAGVisitor {
public:
    virtual void visitDag(DAG *dag) final;

    virtual void visit(DAGCollection * /*op*/) {}
    virtual void visit(DAGRange * /*op*/) {}
    virtual void visit(DAGFilter * /*op*/) {}
    virtual void visit(DAGJoin * /*op*/) {}
    virtual void visit(DAGMap * /*op*/) {}
    virtual void visit(DAGReduce * /*op*/) {}
    virtual void visit(DAGReduceByKey * /*op*/) {}
    virtual void visit(DAGCartesian * /*op*/) {}
};

#endif  // UTILS_DAGVISITOR_H
