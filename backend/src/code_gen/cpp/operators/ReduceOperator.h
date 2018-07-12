//
// Created by sabir on 04.07.17.
//

#ifndef CPP_REDUCEOPERATOR_H
#define CPP_REDUCEOPERATOR_H

#include <iostream>
#include <stdexcept>

#include "Operator.h"

template <class Upstream, class Tuple, class Function>
class ReduceOperator : public Operator {
public:
    ReduceOperator(Upstream *const upstream, Function func)
        : upstream_(upstream), has_returned_(false), function_(func) {}

    INLINE Optional<Tuple> next() {
        if (has_returned_) {
            return {};
        }
        while (auto ret = upstream_->next()) {
            acc_ = function_(acc_, ret);
        }
        has_returned_ = true;
        return acc_;
    }

    INLINE void open() {
        upstream_->open();

        if (auto ret = upstream_->next()) {
            acc_ = ret;
        } else {
            throw std::logic_error("Cannot apply reduce on empty input");
        }
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    Tuple acc_;
    bool has_returned_;
    Function function_;
};

template <class Tuple, class Upstream, class Function>
ReduceOperator<Upstream, Tuple, Function> makeReduceOperator(
        Upstream *const upstream, Function func) {
    return ReduceOperator<Upstream, Tuple, Function>(upstream, func);
};

#endif  // CPP_REDUCEOPERATOR_H
