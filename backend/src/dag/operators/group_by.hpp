#ifndef DAG_OPERATORS_GROUP_BY_HPP
#define DAG_OPERATORS_GROUP_BY_HPP

#include "operator.hpp"

class DAGGroupBy : public DAGOperator {
    JITQ_DAGOPERATOR(DAGGroupBy, "groupby");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_GROUP_BY_HPP
