#ifndef DAG_DAGPARTITION_H
#define DAG_DAGPARTITION_H

#include "DAGOperator.h"

class DAGPartition : public DAGOperator {
    JITQ_DAGOPERATOR(DAGPartition, "partition");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGPARTITION_H
