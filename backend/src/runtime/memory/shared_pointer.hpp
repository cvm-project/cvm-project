#ifndef RUNTIME_MEMORY_SHARED_POINTER_HPP
#define RUNTIME_MEMORY_SHARED_POINTER_HPP

#include <atomic>
#include <utility>

#include <cassert>

namespace runtime {
namespace memory {

class RefCounter;

// Increments the given ref_counter if not nullptr and returns the new count.
uint64_t Increment(RefCounter* ref_counter);

// Decrements the given ref_counter if not nullptr and returns the new count.
// Deletes the ref_counter if the count was decreased to 0.
uint64_t Decrement(RefCounter* ref_counter);

class alignas(64) RefCounter {
    friend uint64_t Increment(RefCounter* ref_counter);
    friend uint64_t Decrement(RefCounter* ref_counter);

public:
    explicit RefCounter(void* pointer);
    RefCounter(const RefCounter& other) = delete;
    RefCounter(RefCounter&& other) = delete;
    RefCounter& operator=(const RefCounter& other) = delete;
    RefCounter& operator=(RefCounter&& other) = delete;

    virtual ~RefCounter();

    void* pointer() const;
    int64_t counter() const;

protected:
    int64_t Increment();
    int64_t Decrement();

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
    SharedPointer& operator=(const SharedPointer& other) {
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
    SharedPointer& operator=(SharedPointer&& other) noexcept {
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

    T* get() const { return pointer_; }
    T* operator->() const { return pointer_; }
    T& operator*() const { return *pointer_; }
    T& operator[](const std::size_t idx) const { return pointer_[idx]; }

    /*
     * Type conversion
     */

    // If this is a pointer, creates a new pointer to type U, with which it
    // shares ownership. Returns not-a-pointer otherwise.
    template <typename U>
    SharedPointer<U> as() const {
        return SharedPointer<U>(ref_counter_, reinterpret_cast<U*>(pointer_));
    }

    // If this is a pointer, creates a new pointer to type U, to which it
    // transfers ownership. Returns not-a-pointer otherwise.
    template <typename U>
    SharedPointer<U> into() {
        return SharedPointer<U>(
                std::exchange(ref_counter_, nullptr),
                reinterpret_cast<U*>(std::exchange(pointer_, nullptr)),
                typename SharedPointer<U>::no_increment());
    }

    /*
     * Member access
     */

    RefCounter* ref_counter() const { return ref_counter_; }

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

#endif  // RUNTIME_MEMORY_SHARED_POINTER_HPP
