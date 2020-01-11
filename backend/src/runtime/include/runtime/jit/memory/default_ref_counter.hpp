#ifndef RUNTIME_JIT_MEMORY_DEFAULT_REF_COUNTER_HPP
#define RUNTIME_JIT_MEMORY_DEFAULT_REF_COUNTER_HPP

#include <memory>

#include "shared_pointer.hpp"

namespace runtime {
namespace memory {

template <typename T>
struct DefaultRefCounter : public RefCounter {
    explicit DefaultRefCounter(
            typename std::unique_ptr<T>::pointer const pointer)
        : RefCounter(pointer), pointer_(pointer) {}

    DefaultRefCounter(const DefaultRefCounter& other) = default;
    DefaultRefCounter(DefaultRefCounter&& other) = delete;
    auto operator=(const DefaultRefCounter& other)
            -> DefaultRefCounter& = default;
    auto operator=(DefaultRefCounter&& other) -> DefaultRefCounter& = delete;
    ~DefaultRefCounter() override = default;

private:
    std::unique_ptr<T> pointer_;
};

}  // namespace memory
}  // namespace runtime

#endif  // RUNTIME_JIT_MEMORY_DEFAULT_REF_COUNTER_HPP
