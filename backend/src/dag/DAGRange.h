//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGRANGE_H
#define DAG_DAGRANGE_H

#include "DAGOperator.h"

class DAGRange : public DAGOperatorBase<DAGRange> {
public:
    constexpr static const char *kName = "range_source";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGRANGE_H
