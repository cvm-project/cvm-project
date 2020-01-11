#ifndef DAG_OPERATORS_REDUCE_BY_KEY_GROUPED_HPP
#define DAG_OPERATORS_REDUCE_BY_KEY_GROUPED_HPP

#include "operator.hpp"

class DAGReduceByKeyGrouped : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduceByKeyGrouped, "reduce_by_key_grouped");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_REDUCE_BY_KEY_GROUPED_HPP
