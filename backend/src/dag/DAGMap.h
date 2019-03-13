//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGMAP_H
#define DAG_DAGMAP_H

#include "DAGOperator.h"

class DAGMap : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMap, "map");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGMAP_H
