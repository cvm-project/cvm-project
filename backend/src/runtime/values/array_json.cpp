#include "array_json.hpp"

#include <nlohmann/json.hpp>

#include "array.hpp"
#include "runtime/memory/shared_pointer.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

void from_json(const nlohmann::json &json, Array *const val) {
    auto const data = reinterpret_cast<char *>(json.at("data").get<size_t>());
    auto const rc = reinterpret_cast<memory::RefCounter *>(
            json.at("ref_counter").get<size_t>());
    val->data = memory::SharedPointer<char>(
            // False positive? clang-tidy complains about wrong number of
            // arguments.
            // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
            rc == nullptr ? (new memory::NoOpRefCounter(data)) : rc);
    assert(val->data.ref_counter()->pointer() == val->data.get());

    val->outer_shape = json.at("outer_shape").get<std::vector<size_t>>();
    val->offsets = json.at("offsets").get<std::vector<size_t>>();
    val->shape = json.at("shape").get<std::vector<size_t>>();
}

// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const Array *const val) {
    json.emplace("data", reinterpret_cast<size_t>(val->data.get()));
    json.emplace("ref_counter",
                 reinterpret_cast<size_t>(val->data.ref_counter()));
    json.emplace("outer_shape", val->outer_shape);
    json.emplace("offsets", val->offsets);
    json.emplace("shape", val->shape);
}

}  // namespace values
}  // namespace runtime
