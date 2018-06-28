#include "value_json.hpp"

#include <memory>

#include <json.hpp>

#include "value.hpp"
#include "value_parser.hpp"

namespace runtime {
namespace values {

// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const Value *const val) {
    LoadValueParsers();  // TODO(ingo): load in a central place
    for (const auto &it : ValueParserRegistry::parsers()) {
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

std::unique_ptr<runtime::values::Value>
adl_serializer<std::unique_ptr<runtime::values::Value>>::from_json(
        const nlohmann::json &json) {
    runtime::values::LoadValueParsers();  // TODO(ingo): load in a central place
    return runtime::values::ValueParserRegistry::parser(json.at("type"))
            ->from_json(json);
}

}  // namespace nlohmann
