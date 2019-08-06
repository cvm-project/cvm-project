#ifndef DAG_OPERATORS_PARAMETER_LOOKUP_HPP
#define DAG_OPERATORS_PARAMETER_LOOKUP_HPP

#include "operator.hpp"

class DAGParameterLookup : public DAGOperator {
    JITQ_DAGOPERATOR(DAGParameterLookup, "parameter_lookup");

public:
    size_t num_in_ports() const override { return 0; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_PARAMETER_LOOKUP_HPP
