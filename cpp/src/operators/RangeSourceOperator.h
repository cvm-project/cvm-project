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

    RangeSourceOperator(Tuple start, Tuple end, Tuple step) : start(start), end(end), step(step) {}

    void printName() {
        std::cout << "range op\n";
    }

    void open() {

    }

    Optional<Tuple> next() {
        if (start < end) {
            start += step;
            return start - step;
        }
        return {};
    }

private:
    Tuple start;
    Tuple end;
    Tuple step;
};


#endif //CPP_RANGESOURCEOPERATOR_H
