#ifndef JITQ_DAG_TYPE_FIELD_HPP
#define JITQ_DAG_TYPE_FIELD_HPP

#include <string>
#include <unordered_map>

#include <json.hpp>

#include "type.hpp"
#include "utils/utils.h"

namespace utils {

class FieldVisitor;

}  // namespace utils

namespace dag {
namespace type {

struct FieldType : public Type {
public:
    virtual void to_json(nlohmann::json *json) const = 0;

    ~FieldType() override = default;
};

}  // namespace type
}  // namespace dag

namespace nlohmann {

template <>
struct adl_serializer<raw_ptr<const dag::type::FieldType>> {
    static raw_ptr<const dag::type::FieldType> from_json(
            const nlohmann::json &json);
    // NOLINTNEXTLINE google-runtime-references
    static void to_json(nlohmann::json &json,
                        raw_ptr<const dag::type::FieldType> type);
};

}  // namespace nlohmann
#endif  // JITQ_DAG_TYPE_FIELD_HPP