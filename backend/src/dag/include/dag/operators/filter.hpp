#ifndef DAG_OPERATORS_FILTER_HPP
#define DAG_OPERATORS_FILTER_HPP

#include "operator.hpp"

class DAGFilter : public DAGOperator {
    JITQ_DAGOPERATOR(DAGFilter, "filter");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_FILTER_HPP
