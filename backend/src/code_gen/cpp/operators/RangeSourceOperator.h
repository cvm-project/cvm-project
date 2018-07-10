//
// Created by sabir on 04.07.17.
//

#ifndef CPP_RANGESOURCEOPERATOR_H
#define CPP_RANGESOURCEOPERATOR_H

#include <iostream>

#include "Operator.h"

template <class Upstream, class Tuple>
class RangeSourceOperator : public Operator {
public:
    typedef decltype(Tuple().v0) ValueType;

    RangeSourceOperator(Upstream *const upstream) : upstream(upstream) {}

    void open() {
        upstream->open();

        auto input_tuple = upstream->next().value();
        current = input_tuple.v0;
        to = input_tuple.v1;
        step = input_tuple.v2;

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

#endif  // CPP_RANGESOURCEOPERATOR_H
