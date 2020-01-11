#include "dag/type/tuple.hpp"

#include <utility>

#include <boost/algorithm/string/join.hpp>
#include <boost/polymorphic_pointer_cast.hpp>
#include <nlohmann/json.hpp>

#include "utils/raw_ptr.hpp"

using dag::type::Tuple;

namespace dag::type {

auto Tuple::MakeTuple(const std::vector<const FieldType *> &field_types)
        -> const Tuple * {
    std::unique_ptr<Tuple> ret(new Tuple());
    ret->field_types = field_types;

    const auto str = ret->to_string();
    TypeRegistry::Register(str, std::move(ret));
    return boost::polymorphic_pointer_downcast<const Tuple>(
            TypeRegistry::at(str));
}

auto Tuple::ComputeHeadTuple(size_t head_size) const -> const Tuple * {
    assert(head_size > 0);
    assert(field_types.size() >= head_size);
    auto first = field_types.begin();
    auto last = field_types.begin() + head_size;
    std::vector<const FieldType *> new_vec(first, last);
    return MakeTuple(new_vec);
}

auto Tuple::ComputeTailTuple(size_t head_size) const -> const Tuple * {
    assert(head_size > 0);
    assert(field_types.size() >= head_size);
    auto first = field_types.begin() + head_size;
    auto last = field_types.end();
    std::vector<const FieldType *> new_vec(first, last);
    return MakeTuple(new_vec);
}

auto Tuple::to_string() const -> std::string {
    std::vector<std::string> item_strings;
    for (auto t : this->field_types) {
        item_strings.push_back(t->to_string());
    }
    return "[" + boost::join(item_strings, ",") + "]";
}

void Tuple::from_json(const nlohmann::json &json) {
    assert(field_types.empty());
    assert(json.is_array());
    for (const auto &j : json) {
        field_types.push_back(
                j.get<raw_ptr<const dag::type::FieldType>>().get());
    }
}

void Tuple::to_json(nlohmann::json *json) const { *json = field_types; }

}  // namespace dag::type
