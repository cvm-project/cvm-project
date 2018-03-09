//
// Created by sabir on 29.08.17.
//

#ifndef OPTIMIZE_SIMPLEPREDICATEMOVEAROUND_H
#define OPTIMIZE_SIMPLEPREDICATEMOVEAROUND_H

#include <vector>

#include "dag/DAG.h"
#include "dag/DAGFilter.h"
#include "utils/DAGVisitor.h"

// cppcheck-suppress noConstructor
class SimplePredicateMoveAround : public DAGVisitor {
public:
    void optimize(DAG *dag_);

    void visit(DAGFilter *op) override;

private:
    std::vector<DAGFilter *> filters;
    DAG *dag{};
};

#endif  // OPTIMIZE_SIMPLEPREDICATEMOVEAROUND_H
