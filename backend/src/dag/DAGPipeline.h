#ifndef DAG_DAGPIPELINE_H
#define DAG_DAGPIPELINE_H

#include <nlohmann/json.hpp>

#include "DAGOperator.h"

class DAGPipeline : public DAGOperator {
    JITQ_DAGOPERATOR(DAGPipeline, "pipeline");

public:
    size_t num_in_ports() const override { return num_inputs; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    size_t num_inputs{};
};

#endif  // DAG_DAGPIPELINE_H
