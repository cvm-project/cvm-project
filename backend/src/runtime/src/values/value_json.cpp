#include "value_json.hpp"

#include <memory>

#include <nlohmann/json.hpp>

#include "runtime/jit/values/value.hpp"
#include "values/value_parser.hpp"

namespace runtime {
namespace values {

// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const Value *const val) {
    LoadValueParsers();  // TODO(ingo): load in a central place
    for (const auto &it : ValueParserRegistry::objects()) {
        if (it.second->try_to_json(json, val)) {
            json.emplace("type", it.first);
            return;
        }
    }
    throw std::runtime_error("Unknown value type");
}

}  // namespace values
}  // namespace runtime

namespace nlohmann {

using runtime::values::Value;

auto adl_serializer<std::shared_ptr<Value>>::from_json(
        const nlohmann::json &json) -> std::shared_ptr<Value> {
    return std::shared_ptr<Value>(json.get<std::unique_ptr<Value>>().release());
}

auto adl_serializer<std::unique_ptr<Value>>::from_json(
        const nlohmann::json &json) -> std::unique_ptr<Value> {
    runtime::values::LoadValueParsers();  // TODO(ingo): load in a central place
    return runtime::values::ValueParserRegistry::at(json.at("type"))
            ->from_json(json);
}

}  // namespace nlohmann
