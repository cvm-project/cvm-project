#ifndef DAG_OPERATORS_REDUCE_HPP
#define DAG_OPERATORS_REDUCE_HPP

#include "operator.hpp"

class DAGReduce : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGReduce, "reduce");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_REDUCE_HPP
