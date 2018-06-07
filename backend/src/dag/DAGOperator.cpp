#include "DAGOperator.h"

#include <string>
#include <vector>

#include <json.hpp>

#include "Column.h"
#include "TupleField.h"
#include "utils/utils.h"

std::vector<TupleField> parse_output_type(const std::string &output) {
    std::string type_ = string_replace(output, "(", "");
    type_ = string_replace(type_, ")", "");
    type_ = string_replace(type_, " ", "");
    std::vector<std::string> types = split_string(type_, ",");
    std::vector<TupleField> ret;
    size_t pos = 0;
    for (auto t : types) {
        ret.emplace_back(t, pos);
        pos++;
    }
    return ret;
}

void from_json(const nlohmann::json &json, DAGOperator &op) {
    op.id = json.at("id").get<size_t>();
    if (json.count("func") > 0) {
        op.llvm_ir = json["func"];
    }
    op.output_type = json.at("output_type").get<std::string>();
    op.fields = parse_output_type(op.output_type);

    for (auto &field : op.fields) {
        field.column = Column::makeColumn();
        field.column->addField(&field);
    }

    op.from_json(json);
}

void to_json(nlohmann::json &json, const DAGOperator &op) {
    json.emplace("id", op.id);
    if (!op.llvm_ir.empty()) {
        json.emplace("func", op.llvm_ir);
    }
    json.emplace("output_type", op.output_type);
    json.emplace("op", op.name());
    op.to_json(&json);
}

void to_json(nlohmann::json &json, const DAGOperator *op) {
    to_json(json, *op);
}

void to_json(nlohmann::json &json, const std::unique_ptr<DAGOperator> &op) {
    to_json(json, *op);
}

bool DAGOperator::CanRead(const Column *const c) const {
    for (auto const &f : fields) {
        if (*(f.column) == *c) return true;
    }
    return false;
}

bool DAGOperator::Reads(const Column *const c) const {
    for (auto const &col : read_set) {
        if (*col == *c) return true;
    }
    return false;
}

bool DAGOperator::Writes(const Column *const c) const {
    for (auto const &col : write_set) {
        if (*col == *c) return true;
    }
    return false;
}
