//
// Created by sabir on 04.07.17.
//

#ifndef CPP_REDUCEBYKEYGROUPEDOPERATOR_H
#define CPP_REDUCEBYKEYGROUPEDOPERATOR_H

#include <iostream>
#include <unordered_map>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

/**
 * Groups up the input tuples by key and reduces the values
 * Binary function must be associative, commutative
 * The return type of the function must be the same as its arguments
 *
 * This implementation assumes that the key column is grouped and works in
 * linear time
 *
 */
template <class Upstream, class Tuple, class KeyType, class ValueType,
          class Function>
class ReduceByKeyGroupedOperator {
public:
    Upstream *upstream;
    Function func;

    ReduceByKeyGroupedOperator(Upstream *upstream, Function func)
        : upstream(upstream), func(func){};

    Optional<Tuple> INLINE next() {
        if (lastTuple) {
            auto key = getKey(lastTuple);
            ValueType res = getValue(lastTuple);
            while ((lastTuple = upstream->next())) {
                auto candKey = getKey(lastTuple);
                if (candKey.v0 != key.v0) {
                    break;
                }
                res = func(res, getValue(lastTuple));
            }
            return buildResult(key, res);
        }
        return {};
    }

    void INLINE open() {
        upstream->open();
        lastTuple = upstream->next();
    }

    void INLINE close() { upstream->close(); }

private:
    Optional<Tuple> lastTuple;

    INLINE static constexpr KeyType getKey(const Tuple &t) {
        return *(const_cast<KeyType *>((KeyType *)(&t)));
    }

    INLINE static constexpr ValueType getValue(const Tuple &t) {
        return *((ValueType *)(((char *)&t) + sizeof(KeyType)));
    }

    INLINE static Tuple buildResult(const KeyType &key, const ValueType &val) {
        Tuple res;
        char *resp = (char *)&res;
        *((KeyType *)resp) = key;
        *((ValueType *)(resp + sizeof(KeyType))) = val;
        return res;
    }
};

template <class Tuple, class KeyType, class ValueType, class Upstream,
          class Function>
ReduceByKeyGroupedOperator<Upstream, Tuple, KeyType, ValueType, Function> INLINE
makeReduceByKeyGroupedOperator(Upstream *upstream, Function func) {
    return ReduceByKeyGroupedOperator<Upstream, Tuple, KeyType, ValueType,
                                      Function>(upstream, func);
};

#endif  // CPP_REDUCEBYKEYGROUPEDOPERATOR_H
