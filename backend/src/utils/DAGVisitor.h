//
// Created by sabir on 21.07.17.
//

#ifndef UTILS_DAGVISITOR_H
#define UTILS_DAGVISITOR_H

#include "dag/DAGOperator.h"

class DAG;
class DAGCartesian;
class DAGCollection;
class DAGConstantTuple;
class DAGFilter;
class DAGJoin;
class DAGMap;
class DAGParameterLookup;
class DAGRange;
class DAGReduce;
class DAGReduceByKey;
class DAGReduceByKeyGrouped;

// TODO(ingo): there should be a const version of DAGVisitor
class DAGVisitor {
public:
    template <class OperatorType>
    friend class DAGOperatorBase;

    explicit DAGVisitor(const DAG *const dag) : dag_(dag) {}

    virtual void StartVisit() final;

protected:
    const DAG *dag() { return dag_; }

    virtual void visit(DAGCollection * /*op*/) {}
    virtual void visit(DAGConstantTuple * /*op*/) {}
    virtual void visit(DAGRange * /*op*/) {}
    virtual void visit(DAGFilter * /*op*/) {}
    virtual void visit(DAGJoin * /*op*/) {}
    virtual void visit(DAGMap * /*op*/) {}
    virtual void visit(DAGParameterLookup * /*op*/) {}
    virtual void visit(DAGReduce * /*op*/) {}
    virtual void visit(DAGReduceByKey * /*op*/) {}
    virtual void visit(DAGReduceByKeyGrouped * /*op*/) {}
    virtual void visit(DAGCartesian * /*op*/) {}

private:
    const DAG *const dag_;
};

template <class OperatorType>
void DAGOperatorBase<OperatorType>::accept(DAGVisitor *v) {
    v->visit(reinterpret_cast<OperatorType *>(this));
}

#endif  // UTILS_DAGVISITOR_H
