
#include "DAGRange.h"
#include "utils/DAGVisitor.h"
#include "utils/constants.h"
#include <string>

using std::to_string;

void DAGRange::accept(DAGVisitor &v) {
    v.visit(this);
}

void DAGRange::parse_json(nlohmann::json json){
    //TODO should not have to cast to double
    this->from = to_string((double)json[DAG_RANGE_FROM]);
    this->to =  to_string((double)json[DAG_RANGE_TO]);
    this->step =  to_string((double)json[DAG_RANGE_STEP]);
}