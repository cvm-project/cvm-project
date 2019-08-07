#ifndef VALUES_ARRAY_JSON_HPP
#define VALUES_ARRAY_JSON_HPP

#include <nlohmann/json.hpp>

#include "runtime/jit/values/array.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace values {

void from_json(const nlohmann::json &json, Array *val);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const Array *val);

}  // namespace values
}  // namespace runtime

#endif  // VALUES_ARRAY_JSON_HPP
