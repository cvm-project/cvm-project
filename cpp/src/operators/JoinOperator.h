//
// Created by sabir on 04.07.17.
//

#ifndef CPP_JOINOPERATOR_H
#define CPP_JOINOPERATOR_H

#include <unordered_map>
#include "Operator.h"
#include <vector>
//#include <iostream>
#include "utils/debug_print.h"

using std::vector;

/**
 * for input tuples (K, V) and (K, W) returns (K, V, W)
 * returns every combination in case of repeating keys
 *
 * Current implementation builds map on the first input operator
 *
 */
template<class Upstream1, class Upstream2, class Tuple, class KeyType, class ValueType1, class ValueType2>
class JoinOperator : public Operator {
public:
    Upstream1 *upstream1;
    Upstream2 *upstream2;

    JoinOperator(Upstream1 *upstream1, Upstream2 *upstream2) : upstream1(upstream1), upstream2(upstream2) {};

    Optional<Tuple> INLINE next() {
        if (intermediateTuples != iteratorEnd) {
            auto res = *intermediateTuples;
            intermediateTuples++;
            //build result tuple here
            //from iterator to the ht value vector
            return buildResult(lastKey, res, lastUpstream2);
        }
        while (auto ret = upstream2->next()) {
            auto key = getKey(ret.value);
            if (ht.count(key)) {
                intermediateTuples = ht[key].begin();
                iteratorEnd = ht[key].end();
                lastUpstream2 = getValue2(ret.value);
                lastKey = key;
                return next();
            }
        }
        return {};
    }

    void INLINE open() {
        upstream1->open();
        upstream2->open();
        if (!ht.empty()) {
            return;
        }
        while (auto ret = upstream1->next()) {
            if (ht.count(getKey(ret.value)) > 0) {
                ht[getKey(ret.value)].push_back(getValue1(ret.value));
            }
            else {
                vector<ValueType1> values;
                values.push_back(getValue1(ret.value));
                ht.emplace(getKey(ret.value), values);
            }
        }
    }

    void INLINE close() {
        upstream1->close();
        upstream2->close();
    }

private:

//    struct hash {
//        size_t operator()(const KeyType &x) const {
//            const std::string str =
//                    std::string(reinterpret_cast<const std::string::value_type *>( &x ), sizeof(KeyType));
//            return std::hash<std::string>()(str);
//        }
//    };
//
//    struct pred {
//        bool operator()(const KeyType x1, const KeyType x2) const {
//
//            const std::string str1 =
//                    std::string(reinterpret_cast<const std::string::value_type *>( &x1 ), sizeof(KeyType));
//            const std::string str2 =
//                    std::string(reinterpret_cast<const std::string::value_type *>( &x2 ), sizeof(KeyType));
//            return str1 == str2;
//        }
//    };

    struct hash {
        size_t operator()(const KeyType x) const {
            return std::hash<long>()(*((long *) (&x)));
        }
    };

    struct pred {
        bool operator()(const KeyType x, const KeyType y) const {
            return *((long *) (&x)) == *((long *) (&y));
        }
    };

    std::unordered_map<KeyType, std::vector<ValueType1>, hash, pred> ht;
    ValueType2 lastUpstream2;
    KeyType lastKey;
    typename std::vector<ValueType1>::iterator intermediateTuples;
    typename std::vector<ValueType1>::iterator iteratorEnd;

    template<class UpstreamTuple>
    INLINE static constexpr KeyType getKey(UpstreamTuple &t) {
        return *(const_cast<KeyType *>((KeyType *) (&t)));
    }

    template<class UpstreamTuple>
    INLINE static constexpr ValueType1 getValue1(UpstreamTuple &t) {
        return *((ValueType1 *) (((char *) &t) + sizeof(KeyType)));
    }

    template<class UpstreamTuple>
    INLINE static constexpr ValueType2 getValue2(UpstreamTuple &t) {
        return *((ValueType2 *) (((char *) &t) + sizeof(KeyType)));
    }

    INLINE static Tuple buildResult(KeyType &key, ValueType1 &val1, ValueType2 &val2) {
        Tuple res;
        char *resp = (char *) &res;
        *((KeyType *) resp) = key;
        *((ValueType1 *) (resp + sizeof(KeyType))) = val1;
        *((ValueType2 *) (resp + sizeof(KeyType) + sizeof(ValueType1))) = val2;
        return res;
    }
};


template<class Tuple, class KeyType, class ValueType1, class ValueType2, class Upstream1, class Upstream2>
JoinOperator<Upstream1, Upstream2, Tuple, KeyType, ValueType1, ValueType2>
INLINE makeJoinOperator(Upstream1 *upstream1, Upstream2 *upstream2) {
    return JoinOperator<Upstream1, Upstream2, Tuple, KeyType, ValueType1, ValueType2>(upstream1, upstream2);
};


#endif //CPP_JOINOPERATOR_H
