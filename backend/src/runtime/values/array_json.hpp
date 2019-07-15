#ifndef RUNTIME_VALUES_ARRAY_JSON_HPP
#define RUNTIME_VALUES_ARRAY_JSON_HPP

#include <nlohmann/json.hpp>

#include "array.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

void from_json(const nlohmann::json &json, Array *val);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const Array *val);

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_ARRAY_JSON_HPP
