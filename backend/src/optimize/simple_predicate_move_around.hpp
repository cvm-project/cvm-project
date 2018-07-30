//
// Created by sabir on 29.08.17.
//

#ifndef OPTIMIZE_SIMPLE_PREDICATE_MOVE_AROUND_HPP
#define OPTIMIZE_SIMPLE_PREDICATE_MOVE_AROUND_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class SimplePredicateMoveAround {
public:
    explicit SimplePredicateMoveAround(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_SIMPLE_PREDICATE_MOVE_AROUND_HPP
