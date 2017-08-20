//
// Created by sabir on 04.07.17.
//

#ifndef CPP_REDUCEBYKEYOPERATOR_H
#define CPP_REDUCEBYKEYOPERATOR_H

#include <unordered_map>
#include <iostream>
#include "Operator.h"


/**
 * Groups up the input tuples by key and reduces the values
 * Binary function must be associative, commutative
 * The return type of the function must be the same as its arguments
 *
 * Result tuples are in an arbitrary order
 *
 */
template<class Upstream, class Tuple, class KeyType, class ValueType, class Function>
class ReduceByKeyOperator : public Operator {
public:
    Upstream *upstream;
    Function func;

    ReduceByKeyOperator(Upstream *upstream, Function func) : upstream(upstream), func(func) {};

    Optional<Tuple> INLINE next() {
        if (tupleIterator != ht.end()) {
            auto res = *tupleIterator;
            tupleIterator++;
            return buildResult(res.first, res.second);
        }

        return {};
    }

    /**
     * read all upstream input
     * reduce in place
     */
    void INLINE open() {
        upstream->open();
        while (auto ret = upstream->next()) {
            auto key = getKey(ret.value);
            auto it = ht.find(key);
            if (it != ht.end()) {
                auto t = it->second;
                it->second = func(t, getValue(ret.value));
            }
            else {
                ht.emplace(key, getValue(ret.value));
            }
        }
        tupleIterator = ht.begin();
    }

    void INLINE close() {
        upstream->close();
    }

private:

    struct hash {
        size_t operator()(const KeyType &x) const {
            const std::string str =
                    std::string(reinterpret_cast<const std::string::value_type *>( &x ), sizeof(KeyType));
            return std::hash<std::string>()(str);
        }
    };

    struct pred {
        bool operator()(const KeyType x1, const KeyType x2) const {

            const std::string str1 =
                    std::string(reinterpret_cast<const std::string::value_type *>( &x1 ), sizeof(KeyType));
            const std::string str2 =
                    std::string(reinterpret_cast<const std::string::value_type *>( &x2 ), sizeof(KeyType));
            return str1 == str2;
        }
    };

    std::unordered_map<KeyType, ValueType, hash, pred> ht;
    typename std::unordered_map<KeyType, ValueType, hash, pred>::iterator tupleIterator;

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


template<class Tuple, class KeyType, class ValueType, class Upstream,  class Function>
ReduceByKeyOperator<Upstream, Tuple, KeyType, ValueType, Function>
INLINE makeReduceByKeyOperator(Upstream *upstream, Function func) {
    return ReduceByKeyOperator<Upstream, Tuple, KeyType, ValueType, Function>(upstream, func);
};


#endif //CPP_REDUCEBYKEYOPERATOR_H
