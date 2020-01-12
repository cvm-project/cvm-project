#ifndef DAG_OPERATORS_RANGE_HPP
#define DAG_OPERATORS_RANGE_HPP

#include "operator.hpp"

class DAGRange : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGRange, "range_source");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_RANGE_HPP
