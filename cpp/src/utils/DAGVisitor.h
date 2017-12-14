//
// Created by sabir on 21.07.17.
//

#ifndef CPP_DAGVISITOR_H
#define CPP_DAGVISITOR_H

#include "dag/DAGCartesian.h"
#include "dag/DAGCollection.h"
#include "dag/DAGFilter.h"
#include "dag/DAGJoin.h"
#include "dag/DAGMap.h"
#include "dag/DAGOperator.h"
#include "dag/DAGRange.h"
#include "dag/DAGReduce.h"
#include "dag/DAGReduceByKey.h"
#include "utils/debug_print.h"

class DAGVisitor {
public:
    virtual void start_visit(DAGOperator *op) { op->accept(this); }

    virtual void visit(DAGOperator *op) {
        DEBUG_PRINT("visiting undefined op");
        visitPredecessors(op);
    }

    virtual void visit(DAGCollection *op) {
        DEBUG_PRINT("visiting collection");
        visitPredecessors(op);
    }

    virtual void visit(DAGRange *op) {
        DEBUG_PRINT("visiting range");
        visitPredecessors(op);
    }

    virtual void visit(DAGFilter *op) {
        DEBUG_PRINT("visiting filter");
        visitPredecessors(op);
    }

    virtual void visit(DAGJoin *op) {
        DEBUG_PRINT("visiting join");
        visitPredecessors(op);
    }

    virtual void visit(DAGMap *op) {
        DEBUG_PRINT("visiting map");
        visitPredecessors(op);
    }

    virtual void visit(DAGReduce *op) {
        DEBUG_PRINT("visiting reduce");
        visitPredecessors(op);
    }

    virtual void visit(DAGReduceByKey *op) {
        DEBUG_PRINT("visiting reduce_by_key");
        visitPredecessors(op);
    }

    virtual void visit(DAGCartesian *op) {
        DEBUG_PRINT("visiting cartesian");
        visitPredecessors(op);
    }

    void visitPredecessors(DAGOperator *op) {
        for (auto it = op->predecessors.begin(); it < op->predecessors.end();
             it++) {
            (*it)->accept(this);
        }
    }

    void visitSuccessors(DAGOperator *op) {
        for (auto it = op->successors.begin(); it < op->successors.end();
             it++) {
            (*it)->accept(this);
        }
    }
};

#endif  // CPP_DAGVISITOR_H
