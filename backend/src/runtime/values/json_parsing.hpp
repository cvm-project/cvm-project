#ifndef RUNTIME_VALUES_JSON_PARSING_HPP
#define RUNTIME_VALUES_JSON_PARSING_HPP

#include <memory>
#include <string>
#include <vector>

#include "value.hpp"

namespace runtime {
namespace values {

using VectorOfValues = std::vector<std::shared_ptr<values::Value>>;

VectorOfValues ConvertFromJsonString(const char* inputs_str);
std::string ConvertToJsonString(const VectorOfValues& val);

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_JSON_PARSING_HPP
