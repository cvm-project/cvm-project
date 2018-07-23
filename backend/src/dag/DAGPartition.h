#ifndef DAG_DAGPARTITION_H
#define DAG_DAGPARTITION_H

#include "DAGOperator.h"

class DAGPartition : public DAGOperatorBase<DAGPartition> {
public:
    constexpr static const char *kName = "partition";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGPARTITION_H
