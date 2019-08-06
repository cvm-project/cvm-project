#ifndef DAG_OPERATORS_RANGE_HPP
#define DAG_OPERATORS_RANGE_HPP

#include "operator.hpp"

class DAGRange : public DAGOperator {
    JITQ_DAGOPERATOR(DAGRange, "range_source");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_RANGE_HPP
