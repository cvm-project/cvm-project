#ifndef DAG_DAGMATERIALIZEROWVECTOR_H
#define DAG_DAGMATERIALIZEROWVECTOR_H

#include "DAGOperator.h"

class DAGMaterializeRowVector : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMaterializeRowVector, "materialize_row_vector");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGMATERIALIZEROWVECTOR_H
