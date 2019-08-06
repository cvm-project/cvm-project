#ifndef DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP
#define DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP

#include "operator.hpp"

class DAGMaterializeColumnChunks : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMaterializeColumnChunks, "materialize_column_chunks");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP
