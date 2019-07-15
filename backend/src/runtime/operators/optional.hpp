#ifndef RUNTIME_OPERATORS_OPTIONAL_HPP
#define RUNTIME_OPERATORS_OPTIONAL_HPP

#include <cassert>

template <typename T>
class Optional {
public:
    Optional() : initialized_(false) {}
    Optional(T value) : value_(value), initialized_(true) {}

    explicit operator bool() const { return initialized_; }

    operator T() const { return value(); }

    T value() const {
        assert(initialized_);
        return value_;
    }

private:
    T value_;
    bool initialized_;
};

#endif  // RUNTIME_OPERATORS_OPTIONAL_HPP
