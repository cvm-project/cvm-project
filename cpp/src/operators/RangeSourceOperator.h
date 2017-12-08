//
// Created by sabir on 04.07.17.
//

#ifndef CPP_RANGESOURCEOPERATOR_H
#define CPP_RANGESOURCEOPERATOR_H

#include <iostream>

#include "Operator.h"

template<class Tuple>
class RangeSourceOperator : public Operator {
public:

    RangeSourceOperator(Tuple start, Tuple end, Tuple step) : from(start), to(end), step(step) {}

    void printName() {
        std::cout << "range op\n";
    }

    void open() {

    }

    INLINE Optional<Tuple> next() {
        if (from < to) {
            from += step;
            return from - step;
        }
        return {};
    }

private:
    Tuple from;
    Tuple to;
    Tuple step;
};


template<class Tuple>
RangeSourceOperator<Tuple> makeRangeSourceOperator(Tuple from, Tuple to, Tuple step) {
    return RangeSourceOperator<Tuple>(from, to , step);
};


#endif //CPP_RANGESOURCEOPERATOR_H
