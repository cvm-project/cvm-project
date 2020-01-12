#ifndef DAG_OPERATORS_GROUP_BY_HPP
#define DAG_OPERATORS_GROUP_BY_HPP

#include "operator.hpp"

class DAGGroupBy : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGGroupBy, "groupby");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_GROUP_BY_HPP
