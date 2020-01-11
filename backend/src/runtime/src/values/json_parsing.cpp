#include "runtime/jit/values/json_parsing.hpp"

#include <nlohmann/json.hpp>

#include "runtime/jit/values/value.hpp"
#include "values/value_json.hpp"

namespace runtime {
namespace values {

auto ConvertFromJsonString(const char* const inputs_str) -> VectorOfValues {
    return nlohmann::json::parse(inputs_str).get<VectorOfValues>();
}

auto ConvertToJsonString(const VectorOfValues& val) -> std::string {
    return nlohmann::json(val).dump();
}

}  // namespace values
}  // namespace runtime
