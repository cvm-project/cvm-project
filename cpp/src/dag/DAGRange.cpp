
#include "DAGRange.h"

#include <string>

#include "utils/DAGVisitor.h"
#include "utils/constants.h"

using std::to_string;

void DAGRange::accept(DAGVisitor *v) { v->visit(this); }

void DAGRange::parse_json(const nlohmann::json &json) {
    // TODO(sabir): should not have to cast to double
    this->from = std::to_string(static_cast<double>(json[DAG_RANGE_FROM]));
    this->to = std::to_string(static_cast<double>(json[DAG_RANGE_TO]));
    this->step = std::to_string(static_cast<double>(json[DAG_RANGE_STEP]));
}
