#ifndef DAG_OPERATORS_EXCHANGE_TCP_HPP
#define DAG_OPERATORS_EXCHANGE_TCP_HPP

#include "operator.hpp"

class DAGExchangeTcp : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGExchangeTcp, "exchange_tcp");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    size_t num_levels{};
    size_t level_num{};
};

#endif  // DAG_OPERATORS_EXCHANGE_TCP_HPP
