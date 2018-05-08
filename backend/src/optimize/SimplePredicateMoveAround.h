//
// Created by sabir on 29.08.17.
//

#ifndef OPTIMIZE_SIMPLEPREDICATEMOVEAROUND_H
#define OPTIMIZE_SIMPLEPREDICATEMOVEAROUND_H

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class SimplePredicateMoveAround {
public:
    explicit SimplePredicateMoveAround(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_SIMPLEPREDICATEMOVEAROUND_H
