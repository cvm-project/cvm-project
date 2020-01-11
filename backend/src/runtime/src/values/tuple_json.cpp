#include "values/tuple_json.hpp"

#include <nlohmann/json.hpp>

#include "runtime/jit/values/tuple.hpp"
#include "runtime/jit/values/value.hpp"
#include "values/value_json.hpp"

namespace runtime::values {

void from_json(const nlohmann::json &json, Tuple *const val) {
    val->fields = json.at("fields").get<std::vector<std::unique_ptr<Value>>>();
}

// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const Tuple *const val) {
    json.emplace("fields", val->fields);
}

}  // namespace runtime::values
