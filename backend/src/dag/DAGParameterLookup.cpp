#include "DAGParameterLookup.h"

#include <json.hpp>

void DAGParameterLookup::from_json(const nlohmann::json &json) {
    this->parameter_num = json.at("parameter_num").get<size_t>();
}

void DAGParameterLookup::to_json(nlohmann::json *json) const {
    json->emplace("parameter_num", this->parameter_num);
}
