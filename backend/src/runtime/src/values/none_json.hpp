#ifndef VALUES_NONE_JSON_HPP
#define VALUES_NONE_JSON_HPP

#include <nlohmann/json.hpp>

#include "runtime/jit/values/tuple.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace values {

inline void from_json(const nlohmann::json& /*json*/, None* /*val*/) {}
// NOLINTNEXTLINE(google-runtime-references)
inline void to_json(nlohmann::json& /*json*/, const None* /*val*/) {}

}  // namespace values
}  // namespace runtime

#endif  // VALUES_NONE_JSON_HPP
