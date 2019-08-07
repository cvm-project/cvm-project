#ifndef RUNTIME_MEMORY_VALUES_HPP
#define RUNTIME_MEMORY_VALUES_HPP

#include <string>

#include "runtime/jit/values/json_parsing.hpp"

namespace runtime {
namespace memory {

void Decrement(const std::string &values);
void Decrement(const values::VectorOfValues &values);

void Increment(const std::string &values);
void Increment(const values::VectorOfValues &values);

}  // namespace memory
}  // namespace runtime

#endif  // RUNTIME_MEMORY_VALUES_HPP
