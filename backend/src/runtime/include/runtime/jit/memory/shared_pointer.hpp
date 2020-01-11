#ifndef RUNTIME_JIT_MEMORY_SHARED_POINTER_HPP
#define RUNTIME_JIT_MEMORY_SHARED_POINTER_HPP

#include <cassert>

#include <atomic>
#include <utility>

namespace runtime {
namespace memory {

class RefCounter;

// Increments the given ref_counter if not nullptr and returns the new count.
auto Increment(RefCounter* ref_counter) -> uint64_t;

// Decrements the given ref_counter if not nullptr and returns the new count.
// Deletes the ref_counter if the count was decreased to 0.
auto Decrement(RefCounter* ref_counter) -> uint64_t;

class alignas(64) RefCounter {
    friend auto Increment(RefCounter* ref_counter) -> uint64_t;
    friend auto Decrement(RefCounter* ref_counter) -> uint64_t;

public:
    explicit RefCounter(void* pointer);
    RefCounter(const RefCounter& other) = delete;
    RefCounter(RefCounter&& other) = delete;
    auto operator=(const RefCounter& other) -> RefCounter& = delete;
    auto operator=(RefCounter&& other) -> RefCounter& = delete;

    virtual ~RefCounter();

    [[nodiscard]] auto pointer() const -> void*;
    [[nodiscard]] auto counter() const -> int64_t;

protected:
    auto Increment() -> int64_t;
    auto Decrement() -> int64_t;

    void* pointer_;

private:
    std::atomic_int64_t counter_;
};

struct NoOpRefCounter : public RefCounter {
    explicit NoOpRefCounter(void* const pointer) : RefCounter(pointer) {}
};

template <typename T>
class SharedPointer {
    template <typename U>
    friend class SharedPointer;

public:
    /*
     * Constructors, destructor, and assignment operators (rule of five)
     */

    // Constructs not-a-pointer.
    SharedPointer() = default;

    // Conctructs a pointer with a given counter and increments that counter.
    explicit SharedPointer(RefCounter* const ref_counter) noexcept
        : SharedPointer(
                  ref_counter,
                  ref_counter == nullptr
                          ? nullptr
                          : reinterpret_cast<T*>(ref_counter->pointer())) {}

    // If provided with a pointer, constructs a pointer from a given pointer
    // and increments the existing counter. Otherwise, constructs not-a-pointer.
    SharedPointer(const SharedPointer& other) noexcept
        : SharedPointer(other.ref_counter_) {}

protected:
    struct no_increment {};

    // Conctructs a pointer with a given counter, but does not increment that
    // counter, i.e., ownership is transfered into the new pointer.
    SharedPointer(RefCounter* const ref_counter, T* const pointer,
                  const no_increment /*tag*/) noexcept
        : ref_counter_(ref_counter), pointer_(pointer) {}

public:
    // Conctructs a pointer with a given counter and increments that counter.
    SharedPointer(RefCounter* const ref_counter, T* const pointer) noexcept
        : ref_counter_(ref_counter), pointer_(pointer) {
        Increment();
    }

    // Moves ownership from other, if other is a pointer.
    SharedPointer(SharedPointer&& other) noexcept {
        ref_counter_ = std::exchange(other.ref_counter_, nullptr);
        pointer_ = std::exchange(other.pointer_, nullptr);
    }

    // If this was a pointer, decrements its counter (which may delete the
    // counter and pointee). If provided with a pointer, assigns that pointer
    // to this and increments its counter. Otherwise, assigns not-a-pointer.
    auto operator=(const SharedPointer& other) -> SharedPointer& {
        // Update this, saving current state
        auto tmp_pointer = std::exchange(pointer_, other.pointer_);
        auto tmp_ref_counter = std::exchange(ref_counter_, other.ref_counter_);

        // Take new ownership, give up old one
        Increment();
        Decrement(&tmp_ref_counter, &tmp_pointer);
        return *this;
    }

    // If this was a pointer, decrements its counter (which may delete the
    // counter and pointee). If provided with a pointer, assigns that pointer
    // to this and takes over ownership. Otherwise, assigns not-a-pointer.
    // Leaves other as not-a-pointer.
    auto operator=(SharedPointer&& other) noexcept -> SharedPointer& {
        if (other.pointer_ != pointer_) {
            Decrement();
        }
        pointer_ = std::exchange(other.pointer_, nullptr);
        ref_counter_ = std::exchange(other.ref_counter_, nullptr);
        return *this;
    }

    // If this was a pointer, decrements its counter (which may delete the
    // counter and pointee).
    ~SharedPointer() { this->Decrement(); }

    /*
     * Pointer interface
     */

    [[nodiscard]] auto get() const -> T* { return pointer_; }
    auto operator-> () const -> T* { return pointer_; }
    auto operator*() const -> T& { return *pointer_; }
    auto operator[](const std::size_t idx) const -> T& { return pointer_[idx]; }

    /*
     * Type conversion
     */

    // If this is a pointer, creates a new pointer to type U, with which it
    // shares ownership. Returns not-a-pointer otherwise.
    template <typename U>
    [[nodiscard]] auto as() const -> SharedPointer<U> {
        return SharedPointer<U>(ref_counter_, reinterpret_cast<U*>(pointer_));
    }

    // If this is a pointer, creates a new pointer to type U, to which it
    // transfers ownership. Returns not-a-pointer otherwise.
    template <typename U>
    auto into() -> SharedPointer<U> {
        return SharedPointer<U>(
                std::exchange(ref_counter_, nullptr),
                reinterpret_cast<U*>(std::exchange(pointer_, nullptr)),
                typename SharedPointer<U>::no_increment());
    }

    /*
     * Member access
     */

    [[nodiscard]] auto ref_counter() const -> RefCounter* {
        return ref_counter_;
    }

private:
    static void Decrement(RefCounter** const ref_counter_ptr,
                          T** const pointer_ptr) {
        if (::runtime::memory::Decrement(*ref_counter_ptr) == 0) {
            *ref_counter_ptr = nullptr;
            *pointer_ptr = nullptr;
        }
    }

    void Decrement() { this->Decrement(&ref_counter_, &pointer_); }
    void Increment() { ::runtime::memory::Increment(ref_counter_); }

    RefCounter* ref_counter_ = nullptr;
    T* pointer_ = nullptr;
};

}  // namespace memory
}  // namespace runtime

#endif  // RUNTIME_JIT_MEMORY_SHARED_POINTER_HPP
