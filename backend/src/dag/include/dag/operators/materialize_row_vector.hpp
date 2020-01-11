#ifndef DAG_OPERATORS_MATERIALIZE_ROW_VECTOR_HPP
#define DAG_OPERATORS_MATERIALIZE_ROW_VECTOR_HPP

#include "operator.hpp"

class DAGMaterializeRowVector : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMaterializeRowVector, "materialize_row_vector");

public:
    auto num_in_ports() const -> size_t override { return 1; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_MATERIALIZE_ROW_VECTOR_HPP
