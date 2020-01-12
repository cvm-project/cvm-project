#ifndef DAG_OPERATORS_SPLIT_RANGE_HPP
#define DAG_OPERATORS_SPLIT_RANGE_HPP

#include "operator.hpp"

class DAGSplitRange : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGSplitRange, "split_range");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 2; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_SPLIT_RANGE_HPP
