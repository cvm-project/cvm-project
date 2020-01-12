#ifndef DAG_OPERATORS_PARTITION_HPP
#define DAG_OPERATORS_PARTITION_HPP

#include "operator.hpp"

class DAGPartition : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGPartition, "partition");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_PARTITION_HPP
