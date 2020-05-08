#include "dag/operators/join.hpp"

void DAGJoin::to_json(nlohmann::json *json) const {
    json->emplace("num_keys", this->num_keys);
}

void DAGJoin::from_json(const nlohmann::json &json) {
    this->num_keys = json.at("num_keys");
}
