#ifndef DAG_OPERATORS_PARALLEL_MAP_HPP
#define DAG_OPERATORS_PARALLEL_MAP_HPP

#include "operator.hpp"

class DAGParallelMap : public DAGOperator {
    JITQ_DAGOPERATOR(DAGParallelMap, "parallel_map");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_PARALLEL_MAP_HPP
