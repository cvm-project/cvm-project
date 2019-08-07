#ifndef RUNTIME_JIT_VALUES_ATOMICS_HPP
#define RUNTIME_JIT_VALUES_ATOMICS_HPP

#include <string>

#include "value.hpp"

namespace runtime {
namespace values {

template <typename InnerType>
struct Atomic : public Value {
    InnerType value{};
};

using Float = Atomic<float>;
using Double = Atomic<double>;
using Int32 = Atomic<int32_t>;
using Int64 = Atomic<int64_t>;
using Bool = Atomic<bool>;
using String = Atomic<std::string>;

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_JIT_VALUES_ATOMICS_HPP
