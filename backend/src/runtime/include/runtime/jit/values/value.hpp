#ifndef RUNTIME_JIT_VALUES_VALUE_HPP
#define RUNTIME_JIT_VALUES_VALUE_HPP

#include <cassert>

#include <type_traits>

namespace runtime {
namespace values {

struct Value {
    virtual ~Value() = default;

    template <typename ConcreteValueType>
    const ConcreteValueType *as() const {
        static_assert(std::is_base_of<Value, ConcreteValueType>::value,
                      "Can only cast to types inheriting from Value.");
        auto ret = dynamic_cast<const ConcreteValueType *>(this);
        assert(ret != nullptr);
        return ret;
    }
    template <typename ConcreteValueType>
    ConcreteValueType *as() {
        return const_cast<ConcreteValueType *>(
                static_cast<const Value *>(this)->as<ConcreteValueType>());
    }
};

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_JIT_VALUES_VALUE_HPP
