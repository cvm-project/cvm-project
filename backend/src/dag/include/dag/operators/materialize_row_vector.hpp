#ifndef DAG_OPERATORS_MATERIALIZE_ROW_VECTOR_HPP
#define DAG_OPERATORS_MATERIALIZE_ROW_VECTOR_HPP

#include "operator.hpp"

class DAGMaterializeRowVector : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGMaterializeRowVector, "materialize_row_vector");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_MATERIALIZE_ROW_VECTOR_HPP
