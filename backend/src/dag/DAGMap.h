//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGMAP_H
#define DAG_DAGMAP_H

#include <iostream>

#include "DAGOperator.h"

class DAGMap : public DAGOperatorBase<DAGMap> {
public:
    constexpr static const char *kName = "map";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGMAP_H
