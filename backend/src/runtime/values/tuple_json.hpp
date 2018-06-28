#ifndef RUNTIME_VALUES_TUPLE_JSON_HPP
#define RUNTIME_VALUES_TUPLE_JSON_HPP

#include <json.hpp>

#include "tuple.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

void from_json(const nlohmann::json &json, Tuple *val);
// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const Tuple *val);

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_TUPLE_JSON_HPP
