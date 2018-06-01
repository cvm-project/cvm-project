#include "DAGOperator.h"

#include "dag/type/array.hpp"

using dag::collection::Tuple;

void from_json(const nlohmann::json &json, DAGOperator &op) {
    op.id = json["id"].get<size_t>();
    if (json.count("func") > 0) {
        op.llvm_ir = json["func"];
    }
    op.tuple = std::make_shared<Tuple>(json.at("output_type"));
    op.from_json(json);
}

void to_json(nlohmann::json &json, const DAGOperator &op) {
    json.emplace("id", op.id);
    if (!op.llvm_ir.empty()) {
        json.emplace("func", op.llvm_ir);
    }
    nlohmann::json tuple_json(make_raw(op.tuple->type));
    json.emplace("output_type", tuple_json);
    json.emplace("op", op.name());
    op.to_json(&json);
}

void to_json(nlohmann::json &json, const DAGOperator *op) {
    to_json(json, *op);
}

void to_json(nlohmann::json &json, const std::unique_ptr<DAGOperator> &op) {
    to_json(json, *op);
}

bool DAGOperator::HasInOutput(const dag::AttributeId *const attribute) const {
    for (auto const &f : tuple->fields) {
        if (*(f->attribute_id()) == *attribute) return true;
    }
    return false;
}

bool DAGOperator::Reads(const dag::AttributeId *attribute) const {
    for (auto const &col : read_set) {
        if (*col == *attribute) return true;
    }
    return false;
}

bool DAGOperator::Writes(const dag::AttributeId *attribute) const {
    for (auto const &col : write_set) {
        if (*col == *attribute) return true;
    }
    return false;
}
