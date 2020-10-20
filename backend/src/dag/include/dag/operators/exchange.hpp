#ifndef DAG_OPERATORS_EXCHANGE_HPP
#define DAG_OPERATORS_EXCHANGE_HPP

#include "operator.hpp"

class DAGExchange : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGExchange, "exchange");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_EXCHANGE_HPP
