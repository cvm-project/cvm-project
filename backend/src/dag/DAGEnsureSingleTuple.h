#ifndef DAG_DAGSINGLETUPLE_H
#define DAG_DAGSINGLETUPLE_H

#include <iostream>

#include "DAGOperator.h"

class DAGEnsureSingleTuple : public DAGOperatorBase<DAGEnsureSingleTuple> {
public:
    constexpr static const char *kName = "ensure_single_tuple";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGSINGLETUPLE_H
