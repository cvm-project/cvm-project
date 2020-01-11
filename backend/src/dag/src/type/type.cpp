#include "dag/type/type.hpp"

#include <memory>

#include <nlohmann/json.hpp>

#include "dag/type/array.hpp"
#include "dag/type/atomic.hpp"
#include "dag/type/tuple.hpp"

using dag::type::Type;
using dag::type::TypeRegistry;

namespace dag {
namespace type {

auto MakeType(std::unique_ptr<const Type> &&type) -> const Type * {
    const auto str = type->to_string();
    TypeRegistry::Register(str, std::move(type));
    return TypeRegistry::at(str);
}

}  // namespace type
}  // namespace dag

auto nlohmann::adl_serializer<std::unique_ptr<Type>>::from_json(
        const nlohmann::json &json) -> std::unique_ptr<Type> {
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
