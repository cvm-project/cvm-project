#include "type.hpp"

#include <memory>

#include <nlohmann/json.hpp>

#include "array.hpp"
#include "atomic.hpp"
#include "tuple.hpp"
#include "utils/utils.h"

using dag::type::Type;
using dag::type::TypeRegistry;

namespace dag {
namespace type {

const Type *MakeType(std::unique_ptr<const Type> &&type) {
    const auto str = type->to_string();
    TypeRegistry::Register(str, std::move(type));
    return TypeRegistry::at(str);
}

}  // namespace type
}  // namespace dag

std::unique_ptr<Type> nlohmann::adl_serializer<
        std::unique_ptr<Type>>::from_json(const nlohmann::json &json) {
    if (json.is_array()) {
        return std::unique_ptr<Type>(new dag::type::Tuple);
    }
    const std::string type = json.at("type");
    if (type == "array") {
        return std::unique_ptr<Type>(new dag::type::Array);
    }
    return std::unique_ptr<Type>(new dag::type::Atomic);
}

namespace nlohmann {

void to_json(nlohmann::json &json, const dag::type::Type *type) {
    type->to_json(&json);
}

}  // namespace nlohmann
