//
// Created by sabir on 22.05.18.
//

#ifndef CPP_DAG_TYPE_TUPLE_HPP
#define CPP_DAG_TYPE_TUPLE_HPP

#include <string>
#include <vector>

#include <json.hpp>

#include "field_type.hpp"
#include "utils/utils.h"

namespace dag {
namespace type {

struct Tuple : public Type {
private:
    Tuple() = default;

public:
    static const Tuple *MakeTuple(
            const std::vector<const FieldType *> &field_types);

    std::string to_string() const override;

    std::vector<const FieldType *> field_types;
};

}  // namespace type
}  // namespace dag

namespace nlohmann {

template <>
struct adl_serializer<raw_ptr<const dag::type::Tuple>> {
    static raw_ptr<const dag::type::Tuple> from_json(const json &json);
    // NOLINTNEXTLINE google-runtime-references
    static void to_json(json &json, raw_ptr<const dag::type::Tuple> tuple);
};

}  // namespace nlohmann

#endif  // CPP_DAG_TYPE_TUPLE_HPP
