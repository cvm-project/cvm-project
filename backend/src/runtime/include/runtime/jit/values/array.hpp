#ifndef RUNTIME_JIT_VALUES_ARRAY_HPP
#define RUNTIME_JIT_VALUES_ARRAY_HPP

#include <cstdint>
#include <vector>

#include "runtime/jit/memory/shared_pointer.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

struct Array : public Value {
    memory::SharedPointer<char> data;
    std::vector<std::size_t> outer_shape;
    std::vector<std::size_t> offsets;
    std::vector<std::size_t> shape;
};

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_JIT_VALUES_ARRAY_HPP
