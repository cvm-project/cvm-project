//
// Created by sabir on 26.08.17.
//

#ifndef IR_ANALYZER_SCHEMAINFERENCE_H
#define IR_ANALYZER_SCHEMAINFERENCE_H

#include "dag/DAGOperators.h"
#include "utils/DAGVisitor.h"

/**
 * assign schema for every operator
 * compute read, write and dead sets
 */
class SchemaInference : public DAGVisitor {
public:
    void visit(DAGCartesian *op) override;
    void visit(DAGCollection *op) override;
    void visit(DAGFilter *op) override;
    void visit(DAGJoin *op) override;
    void visit(DAGMap *op) override;
    void visit(DAGRange *op) override;
    void visit(DAGReduceByKey *op) override;
};

#endif  // IR_ANALYZER_SCHEMAINFERENCE_H
