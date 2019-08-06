#include "dag/operators/projection.hpp"

void DAGProjection::from_json(const nlohmann::json &json) {
    this->positions = json.at("positions").get<std::vector<size_t>>();
}

void DAGProjection::to_json(nlohmann::json *json) const {
    json->emplace("positions", this->positions);
}
