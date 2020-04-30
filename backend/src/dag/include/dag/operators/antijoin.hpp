#ifndef DAG_OPERATORS_ANTIJOIN_HPP
#define DAG_OPERATORS_ANTIJOIN_HPP

#include "operator.hpp"

class DAGAntiJoin : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGAntiJoin, "antijoin");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 2; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_ANTIJOIN_HPP
