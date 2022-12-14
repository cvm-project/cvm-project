#ifndef DAG_OPERATORS_NESTED_MAP_HPP
#define DAG_OPERATORS_NESTED_MAP_HPP

#include "operator.hpp"

class DAGNestedMap : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGNestedMap, "nested_map");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_NESTED_MAP_HPP
