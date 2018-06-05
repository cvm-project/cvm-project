#ifndef DAG_DAGREDUCEBYKEYGROUPED_H
#define DAG_DAGREDUCEBYKEYGROUPED_H

#include "DAGOperator.h"

class DAGReduceByKeyGrouped : public DAGOperatorBase<DAGReduceByKeyGrouped> {
public:
    constexpr static const char *kName = "reduce_by_key_grouped";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGREDUCEBYKEYGROUPED_H
