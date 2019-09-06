#ifndef DAG_OPERATORS_PARALLEL_MAP_OMP_HPP
#define DAG_OPERATORS_PARALLEL_MAP_OMP_HPP

#include "operator.hpp"

class DAGParallelMapOmp : public DAGOperator {
    JITQ_DAGOPERATOR(DAGParallelMapOmp, "parallel_map_omp");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_PARALLEL_MAP_OMP_HPP
