#ifndef DAG_OPERATORS_CARTESIAN_HPP
#define DAG_OPERATORS_CARTESIAN_HPP

#include "operator.hpp"

class DAGCartesian : public DAGOperator {
    JITQ_DAGOPERATOR(DAGCartesian, "cartesian");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_CARTESIAN_HPP
