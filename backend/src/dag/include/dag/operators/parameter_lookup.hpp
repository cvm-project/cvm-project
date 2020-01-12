#ifndef DAG_OPERATORS_PARAMETER_LOOKUP_HPP
#define DAG_OPERATORS_PARAMETER_LOOKUP_HPP

#include "operator.hpp"

class DAGParameterLookup : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGParameterLookup, "parameter_lookup");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 0; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_PARAMETER_LOOKUP_HPP
