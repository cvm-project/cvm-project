//
// Created by sabir on 04.07.17.
//

#ifndef CPP_REDUCEOPERATOR_H
#define CPP_REDUCEOPERATOR_H

#include <iostream>
#include <stdexcept>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class Upstream, class Tuple, class Function>
class ReduceOperator {
public:
    ReduceOperator(Upstream *const upstream, Function func)
        : upstream_(upstream), has_returned_(false), function_(func) {}

    INLINE void open() { has_returned_ = false; }

    INLINE Optional<Tuple> next() {
        if (has_returned_) return {};
        has_returned_ = true;

        upstream_->open();

        auto ret = upstream_->next();
        if (!ret) return {};

        Tuple acc = ret;
        while (auto ret = upstream_->next()) {
            acc = function_(acc, ret);
        }

        upstream_->close();

        return acc;
    }

    INLINE void close() {}

private:
    Upstream *const upstream_;
    bool has_returned_;
    Function function_;
};

template <class Tuple, class Upstream, class Function>
ReduceOperator<Upstream, Tuple, Function> makeReduceOperator(
        Upstream *const upstream, Function func) {
    return ReduceOperator<Upstream, Tuple, Function>(upstream, func);
};

#endif  // CPP_REDUCEOPERATOR_H
