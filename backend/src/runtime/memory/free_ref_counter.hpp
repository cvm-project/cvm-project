#ifndef RUNTIME_MEMORY_FREE_REF_COUNTER_HPP
#define RUNTIME_MEMORY_FREE_REF_COUNTER_HPP

#include "shared_pointer.hpp"

namespace runtime {
namespace memory {

template <typename T>
struct FreeRefCounter : public RefCounter {
    explicit FreeRefCounter(void* const pointer, const std::size_t num_elements)
        : RefCounter(pointer), num_elements_(num_elements) {}

    FreeRefCounter(const FreeRefCounter& other) = default;
    FreeRefCounter(FreeRefCounter&& other) = delete;
    FreeRefCounter& operator=(const FreeRefCounter& other) = default;
    FreeRefCounter& operator=(FreeRefCounter&& other) = delete;

    std::size_t num_elements() const { return num_elements; }
    void set_num_elements(const std::size_t num_elements) {
        num_elements_ = num_elements;
    }

protected:
    ~FreeRefCounter() override {
        auto const t_ptr = reinterpret_cast<T*>(pointer_);
        for (std::size_t i = 0; i < num_elements_; i++) {
            t_ptr[i].~T();
        }
        free(pointer_);
    }

private:
    std::size_t num_elements_;
};

}  // namespace memory
}  // namespace runtime

#endif  // RUNTIME_MEMORY_FREE_REF_COUNTER_HPP
