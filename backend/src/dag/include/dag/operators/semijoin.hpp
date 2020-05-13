#ifndef DAG_OPERATORS_SEMIJOIN_HPP
#define DAG_OPERATORS_SEMIJOIN_HPP

#include "operator.hpp"

class DAGSemiJoin : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGSemiJoin, "semijoin");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 2; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_SEMIJOIN_HPP
