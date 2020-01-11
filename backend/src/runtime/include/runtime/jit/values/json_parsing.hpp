#ifndef RUNTIME_JIT_VALUES_JSON_PARSING_HPP
#define RUNTIME_JIT_VALUES_JSON_PARSING_HPP

#include <memory>
#include <string>
#include <vector>

#include "value.hpp"

namespace runtime {
namespace values {

using VectorOfValues = std::vector<std::shared_ptr<values::Value>>;

auto ConvertFromJsonString(const char* inputs_str) -> VectorOfValues;
auto ConvertToJsonString(const VectorOfValues& val) -> std::string;

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_JIT_VALUES_JSON_PARSING_HPP
