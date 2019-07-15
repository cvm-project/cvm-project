#ifndef RUNTIME_VALUES_VALUE_JSON_HPP
#define RUNTIME_VALUES_VALUE_JSON_HPP

#include <memory>

#include <nlohmann/json.hpp>

#include "value.hpp"

namespace runtime {
namespace values {

// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const Value *const val);

}  // namespace values
}  // namespace runtime

namespace nlohmann {

template <>
struct adl_serializer<std::shared_ptr<runtime::values::Value>> {
    static std::shared_ptr<runtime::values::Value> from_json(
            const nlohmann::json &json);

    // NOLINTNEXTLINE(google-runtime-references)
    static void to_json(nlohmann::json &json,
                        const std::shared_ptr<runtime::values::Value> &val) {
        runtime::values::to_json(json, val.get());
    }
};

template <>
struct adl_serializer<std::unique_ptr<runtime::values::Value>> {
    static std::unique_ptr<runtime::values::Value> from_json(
            const nlohmann::json &json);

    // NOLINTNEXTLINE(google-runtime-references)
    static void to_json(nlohmann::json &json,
                        const std::unique_ptr<runtime::values::Value> &val) {
        runtime::values::to_json(json, val.get());
    }
};

}  // namespace nlohmann

#endif  // RUNTIME_VALUES_VALUE_JSON_HPP
