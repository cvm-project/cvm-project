#ifndef DAG_OPERATORS_PARTITION_HPP
#define DAG_OPERATORS_PARTITION_HPP

#include "operator.hpp"

class DAGPartition : public DAGOperator {
    JITQ_DAGOPERATOR(DAGPartition, "partition");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_PARTITION_HPP
