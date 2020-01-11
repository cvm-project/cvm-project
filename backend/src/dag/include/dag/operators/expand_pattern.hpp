#ifndef DAG_OPERATORS_EXPAND_PATTERN_HPP
#define DAG_OPERATORS_EXPAND_PATTERN_HPP

#include "operator.hpp"

class DAGExpandPattern : public DAGOperator {
    JITQ_DAGOPERATOR(DAGExpandPattern, "expand_pattern");

public:
    auto num_in_ports() const -> size_t override { return 2; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_EXPAND_PATTERN_HPP
