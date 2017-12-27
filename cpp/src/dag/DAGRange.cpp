
#include "DAGRange.h"

#include <string>

#include "utils/DAGVisitor.h"
#include "utils/constants.h"

using std::to_string;

void DAGRange::accept(DAGVisitor *v) { v->visit(this); }

void DAGRange::from_json(const nlohmann::json &json) {
    // TODO(sabir): should not have to cast to double
    this->from = std::to_string(static_cast<double>(json.at("from")));
    this->to = std::to_string(static_cast<double>(json.at("to")));
    this->step = std::to_string(static_cast<double>(json.at("step")));
}

void DAGRange::to_json(nlohmann::json *json) const {
    json->emplace("from", this->from);
    json->emplace("to", this->to);
    json->emplace("step", this->step);
}
