#ifndef RUNTIME_FREE_FUNCTION_HPP
#define RUNTIME_FREE_FUNCTION_HPP

#include <string>

#include "runtime/values/json_parsing.hpp"

namespace runtime {
namespace memory {

void FreeValues(const std::string &values);
void FreeValues(const values::VectorOfValues &values);

}  // namespace memory
}  // namespace runtime

#endif  // RUNTIME_FREE_FUNCTION_HPP
