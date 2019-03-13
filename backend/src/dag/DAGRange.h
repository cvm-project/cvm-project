//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGRANGE_H
#define DAG_DAGRANGE_H

#include "DAGOperator.h"

class DAGRange : public DAGOperator {
    JITQ_DAGOPERATOR(DAGRange, "range_source");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGRANGE_H
