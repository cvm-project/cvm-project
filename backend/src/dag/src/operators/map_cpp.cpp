#include "dag/operators/map_cpp.hpp"

void DAGMapCpp::from_json(const nlohmann::json &json) {
    this->function_name = json.at("function_name");
}

void DAGMapCpp::to_json(nlohmann::json *json) const {
    json->emplace("function_name", this->function_name);
}
