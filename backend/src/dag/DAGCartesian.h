//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCARTESIAN_H
#define DAG_DAGCARTESIAN_H

#include "DAGOperator.h"

class DAGCartesian : public DAGOperatorBase<DAGCartesian> {
public:
    constexpr static const char *kName = "cartesian";
    constexpr static size_t kNumInPorts = 2;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGCARTESIAN_H
