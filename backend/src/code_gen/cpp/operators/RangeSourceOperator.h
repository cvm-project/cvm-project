//
// Created by sabir on 04.07.17.
//

#ifndef CPP_RANGESOURCEOPERATOR_H
#define CPP_RANGESOURCEOPERATOR_H

#include <iostream>

#include "Operator.h"

template <class Tuple>
class RangeSourceOperator : public Operator {
public:
    typedef decltype(Tuple().v0) ValueType;

    RangeSourceOperator(ValueType start, ValueType end, ValueType step)
        : from(start), to(end), step(step) {}

    void printName() { std::cout << "range op\n"; }

    void open() { current = from; }
    void close() {}

    INLINE Optional<Tuple> next() {
        if (current < to) {
            current += step;
            return Tuple{current - step};
        }
        return {};
    }

private:
    const ValueType from;
    const ValueType to;
    const ValueType step;
    ValueType current{};
};

template <class Tuple, typename ValueType>
RangeSourceOperator<Tuple> makeRangeSourceOperator(ValueType from, ValueType to,
                                                   ValueType step) {
    return RangeSourceOperator<Tuple>(from, to, step);
};

#endif  // CPP_RANGESOURCEOPERATOR_H
