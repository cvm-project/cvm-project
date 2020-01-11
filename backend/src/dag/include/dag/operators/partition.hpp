#ifndef DAG_OPERATORS_PARTITION_HPP
#define DAG_OPERATORS_PARTITION_HPP

#include "operator.hpp"

class DAGPartition : public DAGOperator {
    JITQ_DAGOPERATOR(DAGPartition, "partition");

public:
    auto num_in_ports() const -> size_t override { return 1; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_PARTITION_HPP
