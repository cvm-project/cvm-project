#ifndef DAG_OPERATORS_ENSURE_SINGLE_TUPLE_HPP
#define DAG_OPERATORS_ENSURE_SINGLE_TUPLE_HPP

#include "operator.hpp"

class DAGEnsureSingleTuple : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGEnsureSingleTuple, "ensure_single_tuple");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_ENSURE_SINGLE_TUPLE_HPP
