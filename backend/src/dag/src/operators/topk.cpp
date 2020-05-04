#include "dag/operators/topk.hpp"

void DAGTopK::to_json(nlohmann::json *json) const {
    json->emplace("num_elements", this->num_elements);
}

void DAGTopK::from_json(const nlohmann::json &json) {
    this->num_elements = json.at("num_elements");
}
