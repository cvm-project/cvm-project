#ifndef DAG_OPERATORS_JOIN_HPP
#define DAG_OPERATORS_JOIN_HPP

#include "operator.hpp"

class DAGJoin : public DAGOperator {
    JITQ_DAGOPERATOR(DAGJoin, "join");

public:
    auto num_in_ports() const -> size_t override { return 2; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_JOIN_HPP
