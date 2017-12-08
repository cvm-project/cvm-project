//
// Created by sabir on 04.07.17.
//

#ifndef CPP_REDUCEOPERATOR_H
#define CPP_REDUCEOPERATOR_H

#include <iostream>
#include <stdexcept>

#include "Operator.h"
#include "utils/timing.h"

template<class Upstream, class Tuple, class Function>
class ReduceOperator : public Operator {
public:
    Upstream *upstream;

    ReduceOperator(Upstream *upstream1, Function func) : upstream(upstream1), function(func) {}

    INLINE Optional<Tuple> next() {
        while (auto ret = upstream->next()) {
            acc = function(acc, ret);
        }
        return acc;
    }

    INLINE void open() {
        upstream->open();

        if (auto ret = upstream->next()) {
            acc = ret;
        } else {
            throw std::logic_error("Cannot apply reduce on empty input");
        }
    }

    INLINE void close() { upstream->close(); }

private:
    Tuple acc;
    Function function;
};

template<class Tuple, class Upstream, class Function>
ReduceOperator<Upstream, Tuple, Function> makeReduceOperator(Upstream *upstream, Function func) {
    return ReduceOperator<Upstream, Tuple, Function>(upstream, func);
};


#endif //CPP_REDUCEOPERATOR_H
