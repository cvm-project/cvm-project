#ifndef DAG_OPERATORS_REDUCE_HPP
#define DAG_OPERATORS_REDUCE_HPP

#include "operator.hpp"

class DAGReduce : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduce, "reduce");

public:
    auto num_in_ports() const -> size_t override { return 1; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_REDUCE_HPP
