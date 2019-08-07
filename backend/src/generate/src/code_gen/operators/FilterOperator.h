//
// Created by sabir on 04.07.17.
//

#ifndef CPP_FILTEROPERATOR_H
#define CPP_FILTEROPERATOR_H

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class Upstream, class Tuple, class Function>
class FilterOperator {
public:
    Upstream *upstream;
    Function function;

    FilterOperator(Upstream *upstream1, Function func)
        : upstream(upstream1), function(func) {}

    INLINE Optional<Tuple> next() {
        while (auto ret = upstream->next()) {
            if (function(ret)) {
                return ret;
            }
        }
        return {};
    }

    INLINE void open() { upstream->open(); }

    INLINE void close() { upstream->close(); }
};

template <class Tuple, class Upstream, class Function>
FilterOperator<Upstream, Tuple, Function> makeFilterOperator(Upstream *upstream,
                                                             Function func) {
    return FilterOperator<Upstream, Tuple, Function>(upstream, func);
};

#endif  // CPP_FILTEROPERATOR_H
