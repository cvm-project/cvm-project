#include "json_parsing.hpp"

#include <json.hpp>

#include "value.hpp"
#include "value_json.hpp"

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
