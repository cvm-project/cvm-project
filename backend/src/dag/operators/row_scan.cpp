#include "row_scan.hpp"

void DAGRowScan::from_json(const nlohmann::json &json) {
    this->add_index = json.at("add_index");
}

void DAGRowScan::to_json(nlohmann::json *json) const {
    json->emplace("add_index", this->add_index);
}
