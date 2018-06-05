//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCE_H
#define DAG_DAGREDUCE_H

#include "DAGOperator.h"

class DAGReduce : public DAGOperatorBase<DAGReduce> {
public:
    constexpr static const char *kName = "reduce";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGREDUCE_H
