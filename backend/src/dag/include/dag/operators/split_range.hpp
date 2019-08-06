#ifndef DAG_OPERATORS_SPLIT_RANGE_HPP
#define DAG_OPERATORS_SPLIT_RANGE_HPP

#include "operator.hpp"

class DAGSplitRange : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitRange, "split_range");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_SPLIT_RANGE_HPP
