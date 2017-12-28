//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCARTESIAN_H
#define DAG_DAGCARTESIAN_H

#include "DAGOperator.h"

class DAGCartesian : public DAGOperatorBase<DAGCartesian> {
public:
    bool stream_right = true;

    constexpr static const char *kName = "cartesian";
};

#endif  // DAG_DAGCARTESIAN_H
