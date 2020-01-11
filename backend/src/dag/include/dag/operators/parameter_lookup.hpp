#ifndef DAG_OPERATORS_PARAMETER_LOOKUP_HPP
#define DAG_OPERATORS_PARAMETER_LOOKUP_HPP

#include "operator.hpp"

class DAGParameterLookup : public DAGOperator {
    JITQ_DAGOPERATOR(DAGParameterLookup, "parameter_lookup");

public:
    auto num_in_ports() const -> size_t override { return 0; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_PARAMETER_LOOKUP_HPP
