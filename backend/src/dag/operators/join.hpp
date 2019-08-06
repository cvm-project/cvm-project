#ifndef DAG_OPERATORS_JOIN_HPP
#define DAG_OPERATORS_JOIN_HPP

#include "operator.hpp"

class DAGJoin : public DAGOperator {
    JITQ_DAGOPERATOR(DAGJoin, "join");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_JOIN_HPP
