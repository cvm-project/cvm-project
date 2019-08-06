#ifndef DAG_OPERATORS_REDUCE_BY_KEY_GROUPED_HPP
#define DAG_OPERATORS_REDUCE_BY_KEY_GROUPED_HPP

#include "operator.hpp"

class DAGReduceByKeyGrouped : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduceByKeyGrouped, "reduce_by_key_grouped");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_REDUCE_BY_KEY_GROUPED_HPP
