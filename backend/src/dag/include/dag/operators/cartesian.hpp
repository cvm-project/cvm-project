#ifndef DAG_OPERATORS_CARTESIAN_HPP
#define DAG_OPERATORS_CARTESIAN_HPP

#include "operator.hpp"

class DAGCartesian : public DAGOperator {
    JITQ_DAGOPERATOR(DAGCartesian, "cartesian");

public:
    auto num_in_ports() const -> size_t override { return 2; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_CARTESIAN_HPP
