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
    std::unique_ptr<Tuple> ret(new Tuple());
    ret->field_types = field_types;

    const auto str = ret->to_string();
    TypeRegistry::Register(str, std::move(ret));
    return boost::polymorphic_pointer_downcast<const Tuple>(
            TypeRegistry::at(str));
}

const Tuple *Tuple::ComputeHeadTuple(size_t head_size) const {
    assert(head_size > 0);
    assert(field_types.size() >= head_size);
    auto first = field_types.begin();
    auto last = field_types.begin() + head_size;
    std::vector<const FieldType *> new_vec(first, last);
    return MakeTuple(new_vec);
}

const Tuple *Tuple::ComputeTailTuple(size_t head_size) const {
    assert(head_size > 0);
    assert(field_types.size() >= head_size);
    auto first = field_types.begin() + head_size;
    auto last = field_types.end();
    std::vector<const FieldType *> new_vec(first, last);
    return MakeTuple(new_vec);
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
