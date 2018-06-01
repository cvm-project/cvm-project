//
// Created by sabir on 22.05.18.
//
#include "tuple.hpp"

#include <utility>

#include <boost/algorithm/string/join.hpp>
#include <boost/polymorphic_pointer_cast.hpp>

#include <json.hpp>

#include "utils/utils.h"

using dag::type::Tuple;

namespace dag {
namespace type {

const Tuple *Tuple::MakeTuple(
        const std::vector<const FieldType *> &field_types) {
    auto ret = new Tuple();
    ret->field_types = field_types;
    return boost::polymorphic_pointer_downcast<const Tuple>(
            registry()
                    .emplace(ret->to_string(),
                             std::unique_ptr<const Tuple>(ret))
                    .first->second.get());
}

std::string Tuple::to_string() const {
    std::vector<std::string> item_strings;
    for (auto t : this->field_types) {
        item_strings.push_back(t->to_string());
    }
    return "[" + boost::join(item_strings, ",") + "]";
}

}  // namespace type
}  // namespace dag

raw_ptr<const Tuple> nlohmann::adl_serializer<raw_ptr<const Tuple>>::from_json(
        const nlohmann::json &json) {
    std::vector<const dag::type::FieldType *> item_types;
    for (const auto &j : json) {
        item_types.push_back(
                j.get<raw_ptr<const dag::type::FieldType>>().get());
    }
    return make_raw(Tuple::MakeTuple(item_types));
}

void nlohmann::adl_serializer<raw_ptr<const Tuple>>::to_json(
        nlohmann::json &json, raw_ptr<const Tuple> tuple_type) {
    for (auto ft : tuple_type->field_types) {
        nlohmann::json t(make_raw(ft));
        json.push_back(t);
    }
}
