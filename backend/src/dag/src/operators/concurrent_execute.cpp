#include "dag/operators/concurrent_execute.hpp"

void DAGConcurrentExecute::to_json(nlohmann::json *json) const {
    json->emplace("num_inputs", this->num_inputs);
    json->emplace("num_outputs", this->num_outputs);
}

void DAGConcurrentExecute::from_json(const nlohmann::json &json) {
    this->num_inputs = json.at("num_inputs");
    this->num_outputs = json.at("num_outputs");
}
