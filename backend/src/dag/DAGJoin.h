//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGJOIN_H
#define DAG_DAGJOIN_H

#include "DAGOperator.h"

class DAGJoin : public DAGOperatorBase<DAGJoin> {
public:
    constexpr static const char *kName = "join";
    constexpr static size_t kNumInPorts = 2;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGJOIN_H
