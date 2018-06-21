//
// Created by sabir on 13.06.18.
//

#include "field_type.hpp"

#include <json.hpp>

#include "array.hpp"
#include "atomic.hpp"
#include "utils/utils.h"

using dag::type::FieldType;

raw_ptr<const FieldType> nlohmann::adl_serializer<
        raw_ptr<const FieldType>>::from_json(const nlohmann::json &json) {
    std::string type = json.at("type");
    if (type == "array") {
        return raw_ptr<const FieldType>(
                json.get<raw_ptr<const dag::type::Array>>().get());
    }
    return raw_ptr<const FieldType>(
            json.get<raw_ptr<const dag::type::Atomic>>().get());
}

void nlohmann::adl_serializer<raw_ptr<const FieldType>>::to_json(
        nlohmann::json &json, raw_ptr<const FieldType> type) {
    type->to_json(&json);
}
