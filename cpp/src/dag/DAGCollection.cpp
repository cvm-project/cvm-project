#include "DAGCollection.h"

#include "utils/DAGVisitor.h"

void DAGCollection::accept(DAGVisitor *v) { v->visit(this); }

void DAGCollection::from_json(const nlohmann::json &json) {
    this->add_index = json.at("add_index");
}

void DAGCollection::to_json(nlohmann::json *json) const {
    json->emplace("add_index", this->add_index);
}
