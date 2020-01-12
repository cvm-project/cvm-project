#ifndef DAG_OPERATORS_REDUCE_BY_KEY_HPP
#define DAG_OPERATORS_REDUCE_BY_KEY_HPP

#include "operator.hpp"

class DAGReduceByKey : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGReduceByKey, "reduce_by_key");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_REDUCE_BY_KEY_HPP
