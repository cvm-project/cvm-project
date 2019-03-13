#ifndef DAG_DAGREDUCEBYKEYGROUPED_H
#define DAG_DAGREDUCEBYKEYGROUPED_H

#include "DAGOperator.h"

class DAGReduceByKeyGrouped : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduceByKeyGrouped, "reduce_by_key_grouped");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGREDUCEBYKEYGROUPED_H
