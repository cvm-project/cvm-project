#ifndef DAG_OPERATORS_EXPAND_PATTERN_HPP
#define DAG_OPERATORS_EXPAND_PATTERN_HPP

#include "operator.hpp"

class DAGExpandPattern : public DAGOperator {
    JITQ_DAGOPERATOR(DAGExpandPattern, "expand_pattern");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_EXPAND_PATTERN_HPP
