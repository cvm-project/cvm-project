#ifndef DAG_DAGEXPANDPETTER_H
#define DAG_DAGEXPANDPETTER_H

#include "DAGOperator.h"

class DAGExpandPattern : public DAGOperator {
    JITQ_DAGOPERATOR(DAGExpandPattern, "expand_pattern");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGEXPANDPETTER_H
