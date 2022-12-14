#ifndef DAG_OPERATORS_PROJECTION_HPP
#define DAG_OPERATORS_PROJECTION_HPP

#include <vector>

#include <nlohmann/json.hpp>

#include "operator.hpp"

class DAGProjection : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGProjection, "projection");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::vector<size_t> positions;
};

#endif  // DAG_OPERATORS_PROJECTION_HPP
