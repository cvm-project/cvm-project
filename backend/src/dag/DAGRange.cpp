
#include "DAGRange.h"

#include <string>

#include <json.hpp>

void DAGRange::from_json(const nlohmann::json &json) {
    // TODO(sabir): should not be stored as string (?)
    this->from = json["from"].dump();
    this->to = json["to"].dump();
    this->step = json["step"].dump();
}

void DAGRange::to_json(nlohmann::json *json) const {
    json->emplace("from", nlohmann::json::parse(this->from));
    json->emplace("to", nlohmann::json::parse(this->to));
    json->emplace("step", nlohmann::json::parse(this->step));
}
