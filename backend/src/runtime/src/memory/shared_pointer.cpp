#include "runtime/jit/memory/shared_pointer.hpp"

#include <cassert>
#include <cstddef>

namespace runtime {
namespace memory {

RefCounter::RefCounter(void* const pointer) : counter_(0), pointer_(pointer) {
    assert((reinterpret_cast<std::size_t>(this) & 63u) == 0);
}

RefCounter::~RefCounter() { assert(counter_.load() == 0); }

auto RefCounter::pointer() const -> void* { return pointer_; }
auto RefCounter::counter() const -> int64_t { return counter_.load(); }

auto RefCounter::Increment() -> int64_t {
    assert(pointer_ != nullptr);
    return counter_.fetch_add(1) + 1;
}

auto RefCounter::Decrement() -> int64_t {
    assert(pointer_ != nullptr);
    const int64_t old_value = counter_.fetch_sub(1);
    assert(old_value > 0);
    return old_value - 1;
}

auto Increment(RefCounter* const ref_counter) -> uint64_t {
    if (ref_counter == nullptr) return -1;
    return ref_counter->Increment();
}

auto Decrement(RefCounter* const ref_counter) -> uint64_t {
    if (ref_counter == nullptr) return -1;
    const uint64_t remaining_refs = ref_counter->Decrement();
    if (remaining_refs == 0) {
        delete ref_counter;
    }
    return remaining_refs;
}

}  // namespace memory
}  // namespace runtime
