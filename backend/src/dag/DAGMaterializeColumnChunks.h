#ifndef DAG_DAGMATERIALIZECOLUMNCHUNKS_H
#define DAG_DAGMATERIALIZECOLUMNCHUNKS_H

#include "DAGOperator.h"

class DAGMaterializeColumnChunks : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMaterializeColumnChunks, "materialize_column_chunks");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGMATERIALIZECOLUMNCHUNKS_H
