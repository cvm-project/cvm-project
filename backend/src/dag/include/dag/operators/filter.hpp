#ifndef DAG_OPERATORS_FILTER_HPP
#define DAG_OPERATORS_FILTER_HPP

#include "operator.hpp"

class DAGFilter : public DAGOperator {
    JITQ_DAGOPERATOR(DAGFilter, "filter");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_FILTER_HPP
