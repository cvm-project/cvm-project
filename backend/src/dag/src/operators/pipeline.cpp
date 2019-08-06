#include "dag/operators/pipeline.hpp"

void DAGPipeline::from_json(const nlohmann::json &json) {
    this->num_inputs = json.at("num_inputs");
}

void DAGPipeline::to_json(nlohmann::json *json) const {
    json->emplace("num_inputs", this->num_inputs);
}
