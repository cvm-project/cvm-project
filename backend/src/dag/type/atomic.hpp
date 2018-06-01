//
// Created by sabir on 11.05.18.
//

#ifndef DAG_TYPE_ATOMIC_HPP
#define DAG_TYPE_ATOMIC_HPP

#include <string>

#include <json.hpp>

#include "field_type.hpp"
#include "utils/utils.h"

namespace dag {
namespace type {

struct Atomic : public FieldType {
private:
    Atomic() = default;

public:
    static const Atomic *MakeAtomic(const std::string &type);

    std::string to_string() const override;
    void to_json(nlohmann::json *json) const override;

    std::string type = "";
};

}  // namespace type
}  // namespace dag

namespace nlohmann {

template <>
struct adl_serializer<raw_ptr<const dag::type::Atomic>> {
    static raw_ptr<const dag::type::Atomic> from_json(
            const nlohmann::json &json);
};

}  // namespace nlohmann
#endif  // DAG_TYPE_ATOMIC_HPP