#include "runtime/jit/memory/shared_pointer.hpp"

#include <memory>
#include <thread>
#include <unordered_map>

#include <gtest/gtest.h>

#include "runtime/jit/memory/free_ref_counter.hpp"

using runtime::memory::FreeRefCounter;
using runtime::memory::RefCounter;
using runtime::memory::SharedPointer;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::unordered_map<int*, std::unique_ptr<int>> pointers;

struct TestRefCounter : public RefCounter {  // NOLINT
    explicit TestRefCounter(void* const pointer) : RefCounter(pointer) {
        auto* const int_ptr = reinterpret_cast<int*>(pointer);
        pointers.emplace(int_ptr, int_ptr);
    }

    ~TestRefCounter() override {
        auto* const int_ptr = reinterpret_cast<int*>(pointer());
        EXPECT_EQ(pointers.count(int_ptr), 1);
        pointers.erase(int_ptr);
    }
};

// cppcheck-suppress missingOverride
TEST(SharedPointerTest, NullConstruct) {  // NOLINT
    SharedPointer<int> ptr(nullptr);
}

// cppcheck-suppress missingOverride
TEST(SharedPointerTest, ConstAccess) {  // NOLINT
    struct S {
        int val;
    };

    const SharedPointer<S> ptr1(new FreeRefCounter<S>(malloc(sizeof(S)), 1));
    new (ptr1.get()) S{42};  // NOLINT(readability-magic-numbers)

    // Special members
    SharedPointer<S> ptr2 = ptr1.as<S>();
    SharedPointer<S> ptr3(ptr1.as<S>());
    EXPECT_EQ(ptr2.get(), ptr1.get());
    EXPECT_EQ(ptr3.get(), ptr1.get());

    // Pointer access
    EXPECT_EQ((*ptr1).val, 42);
    EXPECT_EQ(ptr1->val, 42);
    EXPECT_EQ(ptr1[0].val, 42);
    EXPECT_EQ((ptr1.get())->val, 42);
}

// cppcheck-suppress missingOverride
TEST(SharedPointerTest, SimpleDelete) {  // NOLINT
    int* const int_ptr = new int(42);

    {
        EXPECT_EQ(pointers.count(int_ptr), 0);
        EXPECT_EQ(pointers.size(), 0);
        SharedPointer<int> ptr(new TestRefCounter(int_ptr));
        EXPECT_EQ(pointers.count(int_ptr), 1);
        EXPECT_EQ(pointers.size(), 1);
        EXPECT_EQ(ptr.ref_counter()->counter(), 1);
        EXPECT_EQ(*ptr, *int_ptr);
    }  // ptr gives up ownership and cleans up

    EXPECT_EQ(pointers.count(int_ptr), 0);
    EXPECT_EQ(pointers.size(), 0);
}

// cppcheck-suppress missingOverride
TEST(SharedPointerTest, PointerAccess) {  // NOLINT
    int* const int_ptr = new int(42);

    SharedPointer<int> ptr1(new TestRefCounter(int_ptr));

    EXPECT_EQ(*ptr1, 42);
    EXPECT_EQ(ptr1.get(), int_ptr);

    struct S {
        int v;
    };
    S* const struct_ptr = new S{42};

    SharedPointer<S> ptr2(new TestRefCounter(struct_ptr));

    EXPECT_EQ((*ptr2).v, 42);
    EXPECT_EQ(ptr2->v, 42);
    EXPECT_EQ(ptr2[0].v, 42);
    EXPECT_EQ(ptr2.get(), struct_ptr);
}

// cppcheck-suppress missingOverride
TEST(SharedPointerTest, Copy) {  // NOLINT
    int* const int_ptr1 = new int(1);
    int* const int_ptr2 = new int(2);

    {
        SharedPointer<int> ptr1(new TestRefCounter(int_ptr1));

        EXPECT_EQ(pointers.count(int_ptr1), 1);
        EXPECT_EQ(pointers.size(), 1);
        EXPECT_EQ(ptr1.ref_counter()->counter(), 1);
        EXPECT_EQ(*ptr1, *int_ptr1);

        {
            EXPECT_EQ(pointers.count(int_ptr2), 0);
            EXPECT_EQ(pointers.size(), 1);

            SharedPointer<int> ptr2(new TestRefCounter(int_ptr2));

            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 2);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 1);
            EXPECT_EQ(*ptr2, *int_ptr2);

            {
                // Add ownership through construction
                // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
                SharedPointer<int> ptr3(ptr2);
                EXPECT_EQ(pointers.count(int_ptr2), 1);
                EXPECT_EQ(pointers.size(), 2);
                EXPECT_EQ(ptr3.ref_counter()->counter(), 2);
                EXPECT_EQ(ptr3.ref_counter(), ptr2.ref_counter());
                EXPECT_EQ(*ptr3, *int_ptr2);
            }  // ptr3 gives up ownership again

            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 2);

            {
                // Add ownership through assignment
                // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
                SharedPointer<int> ptr3 = ptr2;
                EXPECT_EQ(pointers.count(int_ptr2), 1);
                EXPECT_EQ(pointers.size(), 2);
                EXPECT_EQ(ptr3.ref_counter()->counter(), 2);
                EXPECT_EQ(ptr3.ref_counter(), ptr2.ref_counter());
                EXPECT_EQ(*ptr3, *int_ptr2);
            }  // ptr3 gives up ownership again

            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 2);

            {
                // Add ownership through conversion
                SharedPointer<uint32_t> ptr3 = ptr2.as<uint32_t>();
                EXPECT_EQ(pointers.count(int_ptr2), 1);
                EXPECT_EQ(pointers.size(), 2);
                EXPECT_EQ(ptr3.ref_counter()->counter(), 2);
                EXPECT_EQ(ptr3.ref_counter(), ptr2.ref_counter());
                EXPECT_EQ(*ptr3, *int_ptr2);
            }  // ptr3 gives up ownership again

            EXPECT_EQ(pointers.count(int_ptr1), 1);
            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 2);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 1);

            // Add ownership through assignment, cleans up other pointer
            ptr1 = ptr2;

            EXPECT_EQ(pointers.count(int_ptr1), 0);
            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 1);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 2);
            EXPECT_EQ(ptr2.ref_counter(), ptr1.ref_counter());
            EXPECT_EQ(*ptr1, *int_ptr2);
            EXPECT_EQ(*ptr2, *int_ptr2);

            // Keep ownership through re-assignment
            ptr1 = ptr2;

            EXPECT_EQ(pointers.count(int_ptr1), 0);
            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 1);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 2);
            EXPECT_EQ(ptr2.ref_counter(), ptr1.ref_counter());
            EXPECT_EQ(*ptr1, *int_ptr2);
            EXPECT_EQ(*ptr2, *int_ptr2);

            // Keep ownership through self-assignment
            ptr1 = ptr1;

            EXPECT_EQ(pointers.count(int_ptr1), 0);
            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 1);
            EXPECT_EQ(ptr1.ref_counter()->counter(), 2);
            EXPECT_EQ(ptr2.ref_counter(), ptr1.ref_counter());
            EXPECT_EQ(*ptr1, *int_ptr2);
            EXPECT_EQ(*ptr2, *int_ptr2);
        }  // ptr2 gives up ownership

        EXPECT_EQ(pointers.count(int_ptr2), 1);
        EXPECT_EQ(pointers.size(), 1);
        EXPECT_EQ(ptr1.ref_counter()->counter(), 1);
        EXPECT_EQ(*ptr1, *int_ptr2);

        // Keep ownership through self-assignment
        ptr1 = ptr1;

        EXPECT_EQ(pointers.count(int_ptr2), 1);
        EXPECT_EQ(pointers.size(), 1);
        EXPECT_EQ(ptr1.ref_counter()->counter(), 1);
        EXPECT_EQ(*ptr1, *int_ptr2);
    }  // ptr1 gives up ownership and cleans up

    EXPECT_EQ(pointers.count(int_ptr2), 0);
    EXPECT_EQ(pointers.size(), 0);
}

// cppcheck-suppress missingOverride
TEST(SharedPointerTest, Move) {  // NOLINT
    int* const int_ptr1 = new int(1);
    int* const int_ptr2 = new int(2);
    int* const int_ptr3 = new int(3);
    int* const int_ptr4 = new int(4);
    int* const int_ptr5 = new int(5);

    {
        SharedPointer<int> ptr1(new TestRefCounter(int_ptr1));

        EXPECT_EQ(pointers.count(int_ptr1), 1);
        EXPECT_EQ(pointers.size(), 1);
        EXPECT_EQ(ptr1.ref_counter()->counter(), 1);
        EXPECT_EQ(*ptr1, *int_ptr1);

        {
            EXPECT_EQ(pointers.count(int_ptr2), 0);
            EXPECT_EQ(pointers.size(), 1);

            SharedPointer<int> ptr2(new TestRefCounter(int_ptr2));

            EXPECT_EQ(pointers.count(int_ptr2), 1);
            EXPECT_EQ(pointers.size(), 2);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 1);
            EXPECT_EQ(*ptr2, *int_ptr2);

            {
                // Move ownership through construction
                SharedPointer<int> ptr3(std::move(ptr2));
                EXPECT_EQ(pointers.count(int_ptr2), 1);
                EXPECT_EQ(pointers.size(), 2);
                EXPECT_EQ(ptr3.ref_counter()->counter(), 1);
                EXPECT_EQ(*ptr3, *int_ptr2);
            }  // ptr3 gives up ownership, cleans up

            EXPECT_EQ(pointers.count(int_ptr2), 0);
            EXPECT_EQ(pointers.size(), 1);

            ptr2 = SharedPointer<int>(new TestRefCounter(int_ptr3));

            EXPECT_EQ(pointers.count(int_ptr3), 1);
            EXPECT_EQ(pointers.size(), 2);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 1);
            EXPECT_EQ(*ptr2, *int_ptr3);

            {
                // Move ownership through assignment
                SharedPointer<int> ptr3 = std::move(ptr2);
                EXPECT_EQ(pointers.count(int_ptr3), 1);
                EXPECT_EQ(pointers.size(), 2);
                EXPECT_EQ(ptr3.ref_counter()->counter(), 1);
                EXPECT_EQ(*ptr3, *int_ptr3);
            }  // ptr3 gives up ownership, cleans up

            EXPECT_EQ(pointers.count(int_ptr3), 0);
            EXPECT_EQ(pointers.size(), 1);

            ptr2 = SharedPointer<int>(new TestRefCounter(int_ptr4));

            EXPECT_EQ(pointers.count(int_ptr4), 1);
            EXPECT_EQ(pointers.size(), 2);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 1);
            EXPECT_EQ(*ptr2, *int_ptr4);

            {
                // Move ownership through conversion
                SharedPointer<uint32_t> ptr3 = ptr2.into<uint32_t>();
                EXPECT_EQ(pointers.count(int_ptr4), 1);
                EXPECT_EQ(pointers.size(), 2);
                EXPECT_EQ(ptr3.ref_counter()->counter(), 1);
                EXPECT_EQ(*ptr3, *int_ptr4);
            }  // ptr3 gives up ownership, cleans up

            EXPECT_EQ(pointers.count(int_ptr4), 0);
            EXPECT_EQ(pointers.size(), 1);

            ptr2 = SharedPointer<int>(new TestRefCounter(int_ptr5));

            EXPECT_EQ(pointers.count(int_ptr5), 1);
            EXPECT_EQ(pointers.size(), 2);
            EXPECT_EQ(ptr2.ref_counter()->counter(), 1);
            EXPECT_EQ(*ptr2, *int_ptr5);

            // Move ownership through assignment, cleans up other pointer
            ptr1 = std::move(ptr2);

            EXPECT_EQ(pointers.count(int_ptr1), 0);
            EXPECT_EQ(pointers.count(int_ptr5), 1);
            EXPECT_EQ(pointers.size(), 1);
            EXPECT_EQ(ptr1.ref_counter()->counter(), 1);
            EXPECT_EQ(*ptr1, *int_ptr5);

            // Keep ownership through self-assignment
            ptr1 = std::move(ptr1);

            EXPECT_EQ(pointers.count(int_ptr5), 1);
            EXPECT_EQ(pointers.size(), 1);
            // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
            EXPECT_EQ(ptr1.ref_counter()->counter(), 1);
            EXPECT_EQ(*ptr1, *int_ptr5);
        }  // Nothing happens here

        EXPECT_EQ(pointers.count(int_ptr5), 1);
        EXPECT_EQ(pointers.size(), 1);
        EXPECT_EQ(ptr1.ref_counter()->counter(), 1);
        EXPECT_EQ(*ptr1, *int_ptr5);
    }  // ptr1 gives up ownership and cleans up

    EXPECT_EQ(pointers.count(int_ptr5), 0);
    EXPECT_EQ(pointers.size(), 0);
}

// cppcheck-suppress missingOverride
TEST(SharedPointerTest, ThreadedMove) {  // NOLINT
    static constexpr size_t kNumIterations = 10000;

    int* const int_ptr = new int(42);
    auto* const rc = new TestRefCounter(int_ptr);
    SharedPointer<int> ptr1(rc);

    int64_t min = 5;  // NOLINT(readability-magic-numbers)
    int64_t max = 0;  // NOLINT(readability-magic-numbers)

    std::atomic<size_t> num_threads_started = 0;

    auto f = [&]() {
        // Wait for the other thread
        num_threads_started.fetch_add(1);
        while (num_threads_started.load() != 2) {
        }

        for (size_t i = 0; i < kNumIterations; i++) {
            min = std::min(rc->counter(), min);
            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            SharedPointer<int> ptr2(ptr1);
            max = std::max(rc->counter(), max);
        }
    };

    std::thread t1(f);
    std::thread t2(f);

    t1.join();
    t2.join();

    EXPECT_EQ(pointers.count(int_ptr), 1);
    EXPECT_EQ(pointers.size(), 1);
    EXPECT_EQ(*ptr1, *int_ptr);
    EXPECT_EQ(min, 1);
    EXPECT_LE(max, 4);
}
