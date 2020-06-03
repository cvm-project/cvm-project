#ifndef DAG_OPERATORS_ANTIJOIN_PREDICATED_HPP
#define DAG_OPERATORS_ANTIJOIN_PREDICATED_HPP

#include "operator.hpp"

class DAGAntiJoinPredicated : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGAntiJoinPredicated, "antijoin_predicated");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 2; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_ANTIJOIN_PREDICATED_HPP
