#ifndef OPTIMIZE_CANONICALIZE_HPP
#define OPTIMIZE_CANONICALIZE_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class Canonicalize {
public:
    explicit Canonicalize(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_CANONICALIZE_HPP
