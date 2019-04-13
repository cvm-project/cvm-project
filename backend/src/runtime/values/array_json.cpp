#include "array_json.hpp"

#include <nlohmann/json.hpp>

#include "array.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

void from_json(const nlohmann::json &json, Array *const val) {
    val->data = reinterpret_cast<void *>(json.at("data").get<size_t>());
    val->outer_shape = json.at("outer_shape").get<std::vector<size_t>>();
    val->offsets = json.at("offsets").get<std::vector<size_t>>();
    val->shape = json.at("shape").get<std::vector<size_t>>();
}

// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const Array *const val) {
    json.emplace("data", reinterpret_cast<size_t>(val->data));
    json.emplace("outer_shape", val->outer_shape);
    json.emplace("offsets", val->offsets);
    json.emplace("shape", val->shape);
}

}  // namespace values
}  // namespace runtime
