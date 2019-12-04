#include "dag/operators/reduce_by_index.hpp"

void DAGReduceByIndex::to_json(nlohmann::json *json) const {
    json->emplace("min", this->min);
    json->emplace("max", this->max);
}

void DAGReduceByIndex::from_json(const nlohmann::json &json) {
    this->min = json.at("min");
    this->max = json.at("max");
}
