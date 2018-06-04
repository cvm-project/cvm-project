#ifndef OPTIMIZE_DETERMINE_SORTEDNESS_HPP
#define OPTIMIZE_DETERMINE_SORTEDNESS_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class DetermineSortedness {
public:
    explicit DetermineSortedness(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_DETERMINE_SORTEDNESS_HPP
