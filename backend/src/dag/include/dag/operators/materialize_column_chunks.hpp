#ifndef DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP
#define DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP

#include "operator.hpp"

class DAGMaterializeColumnChunks : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGMaterializeColumnChunks, "materialize_column_chunks");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_MATERIALIZE_COLUMN_CHUNKS_HPP
