#ifndef RUNTIME_JIT_OPERATORS_OPTIONAL_HPP
#define RUNTIME_JIT_OPERATORS_OPTIONAL_HPP

#include <cassert>

template <typename T>
class Optional {
public:
    Optional() = default;
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE
    Optional(const T &value) : value_(value), initialized_(true) {}

    explicit operator bool() const { return initialized_; }

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    operator T() const { return value(); }

    auto value() const -> const T & {
        assert(initialized_);
        return value_;
    }

private:
    T value_{};
    bool initialized_{false};
};

#endif  // RUNTIME_JIT_OPERATORS_OPTIONAL_HPP
