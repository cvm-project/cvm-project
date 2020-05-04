#ifndef DAG_OPERATORS_TOPK_HPP
#define DAG_OPERATORS_TOPK_HPP

#include "operator.hpp"

class DAGTopK : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGTopK, "topk");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    size_t num_elements{};
};

#endif  // DAG_OPERATORS_TOPK_HPP
