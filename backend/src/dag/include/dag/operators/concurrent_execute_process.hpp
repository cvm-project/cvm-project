#ifndef DAG_OPERATORS_CONCURRENT_EXECUTE_PROCESS_HPP
#define DAG_OPERATORS_CONCURRENT_EXECUTE_PROCESS_HPP

#include "operator.hpp"

class DAGConcurrentExecuteProcess : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGConcurrentExecuteProcess, "concurrent_execute_process");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_CONCURRENT_EXECUTE_PROCESS_HPP
