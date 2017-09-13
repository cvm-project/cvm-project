//
// Created by sabir on 04.07.17.
//

#ifndef CPP_REDUCEBYKEYOPERATOR_H
#define CPP_REDUCEBYKEYOPERATOR_H

#include <unordered_map>
#include <iostream>
#include "utils/timing.h"
#include "Operator.h"


/**
 * Groups up the input tuples by key and reduces the values
 * Binary function must be associative, commutative
 * The return type of the function must be the same as its arguments
 *
 * This implementation assumes that the keycolumn is grouped and works in linear time
 *
 */
template<class Upstream, class Tuple, class KeyType, class ValueType, class Function>
class ReduceByKeyOperator : public Operator {
public:
    Upstream *upstream;
    Function func;

    ReduceByKeyOperator(Upstream *upstream, Function func) : upstream(upstream), func(func) {};

    Optional<Tuple> INLINE next() {
        if (lastTuple) {
            auto key = getKey(lastTuple);
            ValueType res = getValue(lastTuple);
            while (auto up = upstream->next()) {
                auto candKey = getKey(up);
                if (candKey != key) {
                    lastTuple = up;
                    break;
                }
                res = func(res, getValue(up));
            }
            return buildResult(key, res);
        }
        return {};
    }

    void INLINE open() {
        upstream->open();
        lastTuple = upstream->next();
    }

    void INLINE close() {
        upstream->close();

    }

private:

    Optional<Tuple> lastTuple;

    template<class UpstreamTuple>
    INLINE static constexpr KeyType getKey(const UpstreamTuple &t) {
        return *(const_cast<KeyType *>((KeyType *) (&t)));
    }

    template<class UpstreamTuple>
    INLINE static constexpr ValueType getValue(const UpstreamTuple &t) {
        return *((ValueType *) (((char *) &t) + sizeof(KeyType)));
    }

    INLINE static Tuple buildResult(const KeyType &key, const ValueType &val) {
        Tuple res;
        char *resp = (char *) &res;
        *((KeyType *) resp) = key;
        *((ValueType *) (resp + sizeof(KeyType))) = val;
        return res;
    }
};


template<class Tuple, class KeyType, class ValueType, class Upstream, class Function>
ReduceByKeyOperator<Upstream, Tuple, KeyType, ValueType, Function>
INLINE makeReduceByKeyOperator(Upstream *upstream, Function func) {
    return ReduceByKeyOperator<Upstream, Tuple, KeyType, ValueType, Function>(upstream, func);
};


#endif //CPP_REDUCEBYKEYOPERATOR_H
