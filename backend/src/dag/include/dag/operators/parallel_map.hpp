#ifndef DAG_OPERATORS_PARALLEL_MAP_HPP
#define DAG_OPERATORS_PARALLEL_MAP_HPP

#include "operator.hpp"

class DAGParallelMap : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGParallelMap, "parallel_map");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_PARALLEL_MAP_HPP
