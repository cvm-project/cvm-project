//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCEBYKEY_H
#define DAG_DAGREDUCEBYKEY_H

#include "DAGOperator.h"

class DAGReduceByKey : public DAGOperatorBase<DAGReduceByKey> {
public:
    constexpr static const char *kName = "reduce_by_key";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGREDUCEBYKEY_H
