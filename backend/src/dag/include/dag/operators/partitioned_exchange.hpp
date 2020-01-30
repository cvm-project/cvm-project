#ifndef DAG_OPERATORS_PARTITIONED_EXCHANGE_HPP
#define DAG_OPERATORS_PARTITIONED_EXCHANGE_HPP

#include "operator.hpp"

class DAGPartitionedExchange : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGPartitionedExchange, "partitioned_exchange");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    size_t num_levels{};
    size_t level_num{};
};

#endif  // DAG_OPERATORS_PARTITIONED_EXCHANGE_HPP
