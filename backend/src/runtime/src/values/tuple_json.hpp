#ifndef VALUES_TUPLE_JSON_HPP
#define VALUES_TUPLE_JSON_HPP

#include <nlohmann/json.hpp>

#include "runtime/jit/values/tuple.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace values {

void from_json(const nlohmann::json &json, Tuple *val);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const Tuple *val);

}  // namespace values
}  // namespace runtime

#endif  // VALUES_TUPLE_JSON_HPP
