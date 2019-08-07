#ifndef RUNTIME_JIT_OPERATORS_VALUE_OPERATOR_HPP
#define RUNTIME_JIT_OPERATORS_VALUE_OPERATOR_HPP

#include <memory>

#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace operators {

struct ValueOperator {
    virtual void open() {}
    virtual std::shared_ptr<runtime::values::Value> next() = 0;
    virtual void close() {}
    virtual ~ValueOperator() = default;
};

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_VALUE_OPERATOR_HPP
