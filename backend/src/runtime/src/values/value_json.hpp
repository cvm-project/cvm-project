#ifndef VALUES_VALUE_JSON_HPP
#define VALUES_VALUE_JSON_HPP

#include <memory>

#include <nlohmann/json.hpp>

#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace values {

// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const Value *val);

}  // namespace values
}  // namespace runtime

namespace nlohmann {

template <>
struct adl_serializer<std::shared_ptr<runtime::values::Value>> {
    static auto from_json(const nlohmann::json &json)
            -> std::shared_ptr<runtime::values::Value>;

    // NOLINTNEXTLINE(google-runtime-references)
    static void to_json(nlohmann::json &json,
                        const std::shared_ptr<runtime::values::Value> &val) {
        runtime::values::to_json(json, val.get());
    }
};

template <>
struct adl_serializer<std::unique_ptr<runtime::values::Value>> {
    static auto from_json(const nlohmann::json &json)
            -> std::unique_ptr<runtime::values::Value>;

    // NOLINTNEXTLINE(google-runtime-references)
    static void to_json(nlohmann::json &json,
                        const std::unique_ptr<runtime::values::Value> &val) {
        runtime::values::to_json(json, val.get());
    }
};

}  // namespace nlohmann

#endif  // VALUES_VALUE_JSON_HPP
