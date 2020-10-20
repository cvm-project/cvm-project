#ifndef DAG_OPERATORS_CONCURRENT_EXECUTE_HPP
#define DAG_OPERATORS_CONCURRENT_EXECUTE_HPP

#include "operator.hpp"

class DAGConcurrentExecute : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGConcurrentExecute, "concurrent_execute");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override {
        return num_inputs;
    }
    [[nodiscard]] auto num_out_ports() const -> size_t override {
        return num_outputs;
    }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    size_t num_inputs = 1;
    size_t num_outputs = 1;
};

#endif  // DAG_OPERATORS_CONCURRENT_EXECUTE_HPP
