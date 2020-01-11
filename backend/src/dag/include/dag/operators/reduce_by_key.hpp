#ifndef DAG_OPERATORS_REDUCE_BY_KEY_HPP
#define DAG_OPERATORS_REDUCE_BY_KEY_HPP

#include "operator.hpp"

class DAGReduceByKey : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduceByKey, "reduce_by_key");

public:
    auto num_in_ports() const -> size_t override { return 1; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_REDUCE_BY_KEY_HPP
