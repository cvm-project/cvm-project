#include "shared_pointer.hpp"

#include <cassert>
#include <cstddef>

namespace runtime {
namespace memory {

RefCounter::RefCounter(void* const pointer) : counter_(0), pointer_(pointer) {
    assert((reinterpret_cast<std::size_t>(this) & 63u) == 0);
}

RefCounter::~RefCounter() { assert(counter_.load() == 0); }

void* RefCounter::pointer() const { return pointer_; }
int64_t RefCounter::counter() const { return counter_.load(); }

int64_t RefCounter::Increment() {
    assert(pointer_ != nullptr);
    return counter_.fetch_add(1) + 1;
}

int64_t RefCounter::Decrement() {
    assert(pointer_ != nullptr);
    const int64_t old_value = counter_.fetch_sub(1);
    assert(old_value > 0);
    return old_value - 1;
}

uint64_t Increment(RefCounter* const ref_counter) {
    if (ref_counter == nullptr) return -1;
    return ref_counter->Increment();
}

uint64_t Decrement(RefCounter* const ref_counter) {
    if (ref_counter == nullptr) return -1;
    const uint64_t remaining_refs = ref_counter->Decrement();
    if (remaining_refs == 0) {
        delete ref_counter;
    }
    return remaining_refs;
}

}  // namespace memory
}  // namespace runtime
