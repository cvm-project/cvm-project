#include "DAGCollection.h"

#include "utils/DAGVisitor.h"

void DAGCollection::accept(DAGVisitor *v) { v->visit(this); }

void DAGCollection::parse_json(const nlohmann::json &json) {
    this->add_index = json["add_index"];
}
