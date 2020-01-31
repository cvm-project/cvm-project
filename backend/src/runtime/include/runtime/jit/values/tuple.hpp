#ifndef RUNTIME_JIT_VALUES_TUPLE_HPP
#define RUNTIME_JIT_VALUES_TUPLE_HPP

#include <memory>
#include <vector>

#include "value.hpp"

namespace runtime {
namespace values {

struct Tuple : public Value {
    std::vector<std::shared_ptr<Value>> fields;
};

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_JIT_VALUES_TUPLE_HPP
