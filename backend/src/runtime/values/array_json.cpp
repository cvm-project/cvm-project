#include "array_json.hpp"

#include <json.hpp>

#include "array.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

void from_json(const nlohmann::json &json, Array *const val) {
    val->data = reinterpret_cast<void *>(json.at("data").get<size_t>());
    val->shape = json.at("shape").get<std::vector<size_t>>();
}

// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const Array *const val) {
    json.emplace("data", reinterpret_cast<size_t>(val->data));
    json.emplace("shape", val->shape);
}

}  // namespace values
}  // namespace runtime
