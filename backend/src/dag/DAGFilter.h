//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGFILTER_H
#define DAG_DAGFILTER_H

#include "DAGOperator.h"

class DAGFilter : public DAGOperator {
    JITQ_DAGOPERATOR(DAGFilter, "filter");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }

    DAGFilter *copy();
};

#endif  // DAG_DAGFILTER_H
