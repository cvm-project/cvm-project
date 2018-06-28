#ifndef RUNTIME_VALUES_TUPLE_HPP
#define RUNTIME_VALUES_TUPLE_HPP

#include "value.hpp"

#include <memory>
#include <vector>

namespace runtime {
namespace values {

struct Tuple : public Value {
    std::vector<std::unique_ptr<Value>> fields;
};

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_TUPLE_HPP
