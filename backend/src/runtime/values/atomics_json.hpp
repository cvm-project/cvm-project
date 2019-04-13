#ifndef RUNTIME_VALUES_ATOMICS_JSON_HPP
#define RUNTIME_VALUES_ATOMICS_JSON_HPP

#include <cassert>

#include <nlohmann/json.hpp>

#include "atomics.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

template <typename InnerType>
void from_json(const nlohmann::json &json, Atomic<InnerType> *const val) {
    val->value = json.at("value").get<InnerType>();
}

template <typename InnerType>
// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const Atomic<InnerType> *const val) {
    json.emplace("value", val->value);
}

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_ATOMICS_JSON_HPP
