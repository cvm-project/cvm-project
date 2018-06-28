#ifndef RUNTIME_VALUES_ARRAY_HPP
#define RUNTIME_VALUES_ARRAY_HPP

#include <vector>

#include "value.hpp"

namespace runtime {
namespace values {

struct Array : public Value {
    void *data{};
    std::vector<size_t> shape;
};

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_ARRAY_HPP
