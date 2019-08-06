#ifndef DAG_OPERATORS_MAP_HPP
#define DAG_OPERATORS_MAP_HPP

#include "operator.hpp"

class DAGMap : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMap, "map");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_MAP_HPP
