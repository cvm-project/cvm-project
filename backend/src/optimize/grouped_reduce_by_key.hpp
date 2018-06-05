#ifndef OPTIMIZE_GROUPEDREDUCEBYKEY_HPP
#define OPTIMIZE_GROUPEDREDUCEBYKEY_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class GroupedReduceByKey {
public:
    explicit GroupedReduceByKey(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_GROUPEDREDUCEBYKEY_HPP
