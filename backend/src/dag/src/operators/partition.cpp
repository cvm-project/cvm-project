#include "dag/operators/partition.hpp"

void DAGPartition::from_json(const nlohmann::json &json) {
    this->seed = json.at("seed");
}

void DAGPartition::to_json(nlohmann::json *json) const {
    json->emplace("seed", this->seed);
}
