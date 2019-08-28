#ifndef DAG_OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP
#define DAG_OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP

#include "operator.hpp"

class DAGConcurrentExecuteLambda : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGConcurrentExecuteLambda, "concurrent_execute_lambda");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP
