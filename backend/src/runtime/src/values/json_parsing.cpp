#include "runtime/jit/values/json_parsing.hpp"

#include <nlohmann/json.hpp>

#include "runtime/jit/values/value.hpp"
#include "values/value_json.hpp"

namespace runtime {
namespace values {

VectorOfValues ConvertFromJsonString(const char* const inputs_str) {
    return nlohmann::json::parse(inputs_str).get<VectorOfValues>();
}

std::string ConvertToJsonString(const VectorOfValues& val) {
    return nlohmann::json(val).dump();
}

}  // namespace values
}  // namespace runtime
