#ifndef DAG_OPERATORS_ENSURE_SINGLE_TUPLE_HPP
#define DAG_OPERATORS_ENSURE_SINGLE_TUPLE_HPP

#include "operator.hpp"

class DAGEnsureSingleTuple : public DAGOperator {
    JITQ_DAGOPERATOR(DAGEnsureSingleTuple, "ensure_single_tuple");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_ENSURE_SINGLE_TUPLE_HPP
