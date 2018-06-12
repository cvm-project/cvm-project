#ifndef DAG_DAGPARALLELMAP_H
#define DAG_DAGPARALLELMAP_H

#include "DAGOperator.h"

class DAGParallelMap : public DAGOperatorBase<DAGParallelMap> {
public:
    constexpr static const char *kName = "parallel_map";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGPARALLELMAP_H
