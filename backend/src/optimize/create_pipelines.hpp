#ifndef OPTIMIZE_CREATE_PIPELINES_HPP
#define OPTIMIZE_CREATE_PIPELINES_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class CreatePipelines {
public:
    explicit CreatePipelines(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_CREATE_PIPELINES_HPP
