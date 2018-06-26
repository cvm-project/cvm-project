#ifndef OPTIMIZE_TYPE_INFERENCE_HPP
#define OPTIMIZE_TYPE_INFERENCE_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class TypeInference {
public:
    explicit TypeInference(DAG* const dag, const bool only_check = false)
        : dag_(dag), only_check_(only_check) {}

    void optimize();

private:
    DAG* const dag_;
    const bool only_check_;
};

#endif  // OPTIMIZE_TYPE_INFERENCE_HPP
