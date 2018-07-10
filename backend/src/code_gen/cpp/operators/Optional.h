//
// Created by sabir on 18.07.17.
//

#ifndef CPP_OPTIONAL_H
#define CPP_OPTIONAL_H

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

#endif  // CPP_OPTIONAL_H
