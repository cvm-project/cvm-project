#include "constant_tuple.hpp"

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

void DAGConstantTuple::from_json(const nlohmann::json &json) {
    this->values = json.at("values").get<std::vector<std::string>>();
}

void DAGConstantTuple::to_json(nlohmann::json *json) const {
    json->emplace("values", this->values);
}
