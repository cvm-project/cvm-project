//
// Created by sabir on 04.07.17.
//

#ifndef CODE_GEN_OPERATORS_RANGESOURCEOPERATOR_H
#define CODE_GEN_OPERATORS_RANGESOURCEOPERATOR_H

#include <iostream>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class Upstream, class Tuple>
class RangeSourceOperator {
public:
    typedef decltype(Tuple().v0) ValueType;

    RangeSourceOperator(Upstream *const upstream) : upstream(upstream) {}

    void open() {
        upstream->open();

        const auto ret = upstream->next();

        if (!ret) {
            current = to;
        } else {
            const auto input_tuple = ret.value();
            current = input_tuple.v0;
            to = input_tuple.v1;
            step = input_tuple.v2;
        }

        upstream->close();
    }

    void close() {}

    INLINE Optional<Tuple> next() {
        if (current < to) {
            current += step;
            return Tuple{current - step};
        }
        return {};
    }

private:
    Upstream *const upstream;
    ValueType current{};
    ValueType to{};
    ValueType step{};
};

template <class Tuple, class Upstream>
RangeSourceOperator<Upstream, Tuple> makeRangeSourceOperator(
        Upstream *const upstream) {
    return RangeSourceOperator<Upstream, Tuple>(upstream);
};

#endif  // CODE_GEN_OPERATORS_RANGESOURCEOPERATOR_H
