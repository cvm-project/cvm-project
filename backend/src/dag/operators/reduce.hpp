#ifndef DAG_OPERATORS_REDUCE_HPP
#define DAG_OPERATORS_REDUCE_HPP

#include "operator.hpp"

class DAGReduce : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduce, "reduce");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_REDUCE_HPP
