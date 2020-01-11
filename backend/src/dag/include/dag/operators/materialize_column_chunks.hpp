#ifndef DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP
#define DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP

#include "operator.hpp"

class DAGMaterializeColumnChunks : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMaterializeColumnChunks, "materialize_column_chunks");

public:
    auto num_in_ports() const -> size_t override { return 1; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP
