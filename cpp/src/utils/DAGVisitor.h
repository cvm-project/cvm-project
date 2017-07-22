//
// Created by sabir on 21.07.17.
//

#ifndef CPP_DAGVISITOR_H
#define CPP_DAGVISITOR_H

#include "dag/DAGCollection.h"
#include "dag/DAGFilter.h"
#include "dag/DAGJoin.h"
#include "dag/DAGMap.h"
#include "dag/DAGRange.h"
#include "dag/DAGOperator.h"

class DAGVisitor {
public:

    virtual void start_visit(DAGOperator *op) {
        op->accept(*this);
    }

    virtual void visit(DAGOperator *op) {
        std::cout << "visiting undefined op" << std::endl;
        visitPredecessors(op);
    }

    virtual void visit(DAGCollection *op) {
        std::cout << "visiting collection" << std::endl;
        visitPredecessors(op);
    }

    virtual void visit(DAGRange *op) {
        std::cout << "visiting range" << std::endl;
        visitPredecessors(op);
    }

    virtual void visit(DAGFilter *op) {
        std::cout << "visiting filter" << std::endl;
        visitPredecessors(op);
    }

    virtual void visit(DAGJoin *op) {
        std::cout << "visiting join" << std::endl;
        visitPredecessors(op);
    }

    virtual void visit(DAGMap *op) {
        std::cout << "visiting map" << std::endl;
        visitPredecessors(op);
    }

    void visitPredecessors(DAGOperator *op) {
        for (auto it = op->predecessors.begin(); it < op->predecessors.end(); it++) {
            (*it)->accept(*this);
        }
    }

    void visitSuccessors(DAGOperator *op) {
        for (auto it = op->successors.begin(); it < op->successors.end(); it++) {
            (*it)->accept(*this);
        }
    }

};

#endif //CPP_DAGVISITOR_H