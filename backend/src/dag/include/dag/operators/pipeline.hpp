#ifndef DAG_OPERATORS_PIPELINE_HPP
#define DAG_OPERATORS_PIPELINE_HPP

#include <nlohmann/json.hpp>

#include "operator.hpp"

class DAGPipeline : public DAGOperator {
    JITQ_DAGOPERATOR(DAGPipeline, "pipeline");

public:
    auto num_in_ports() const -> size_t override { return num_inputs; }
    auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    size_t num_inputs{};
};

#endif  // DAG_OPERATORS_PIPELINE_HPP
