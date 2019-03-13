#ifndef DAG_DAGPARALLELMAP_H
#define DAG_DAGPARALLELMAP_H

#include "DAGOperator.h"

class DAGParallelMap : public DAGOperator {
    JITQ_DAGOPERATOR(DAGParallelMap, "parallel_map");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGPARALLELMAP_H
