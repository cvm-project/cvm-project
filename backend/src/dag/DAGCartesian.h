//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCARTESIAN_H
#define DAG_DAGCARTESIAN_H

#include "DAGOperator.h"

class DAGCartesian : public DAGOperator {
    JITQ_DAGOPERATOR(DAGCartesian, "cartesian");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGCARTESIAN_H
