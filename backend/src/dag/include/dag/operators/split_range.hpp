#ifndef DAG_OPERATORS_SPLIT_RANGE_HPP
#define DAG_OPERATORS_SPLIT_RANGE_HPP

#include "operator.hpp"

class DAGSplitRange : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitRange, "split_range");

public:
    auto num_in_ports() const -> size_t override { return 2; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_SPLIT_RANGE_HPP
