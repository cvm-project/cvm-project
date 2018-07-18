#ifndef RUNTIME_VALUES_ARRAY_HPP
#define RUNTIME_VALUES_ARRAY_HPP

#include <cstdint>
#include <vector>

#include "value.hpp"

namespace runtime {
namespace values {

struct Array : public Value {
    void *data{};
    std::vector<std::size_t> outer_shape;
    std::vector<std::size_t> offsets;
    std::vector<std::size_t> shape;
};

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_ARRAY_HPP
