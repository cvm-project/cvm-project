#ifndef DAG_DAGSINGLETUPLE_H
#define DAG_DAGSINGLETUPLE_H

#include "DAGOperator.h"

class DAGEnsureSingleTuple : public DAGOperator {
    JITQ_DAGOPERATOR(DAGEnsureSingleTuple, "ensure_single_tuple");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGSINGLETUPLE_H
