#ifndef DAG_OPERATORS_PROJECTION_HPP
#define DAG_OPERATORS_PROJECTION_HPP

#include <vector>

#include <nlohmann/json.hpp>

#include "operator.hpp"

class DAGProjection : public DAGOperator {
    JITQ_DAGOPERATOR(DAGProjection, "projection");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::vector<size_t> positions;
};

#endif  // DAG_OPERATORS_PROJECTION_HPP
