#include "dag/operators/zip.hpp"

void DAGZip::to_json(nlohmann::json *json) const {
    json->emplace("num_inputs", this->num_inputs);
}

void DAGZip::from_json(const nlohmann::json &json) {
    this->num_inputs = json.at("num_inputs");
}
