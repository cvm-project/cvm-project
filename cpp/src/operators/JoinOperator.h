//
// Created by sabir on 04.07.17.
//

#ifndef CPP_JOINOPERATOR_H
#define CPP_JOINOPERATOR_H

#include <iostream>
#include <unordered_map>
#include "Operator.h"

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

    void printName() {
        std::cout << "join op\n";
        upstream1->printName();
        upstream2->printName();
    }

    Optional<Tuple> INLINE next(intermediateTuples) {
        if (intermediateTuples.size() > 0) {
            auto res = intermediateTuples.back();
            intermediateTuples.pop_back();
            //build result tuple here
            //from iterator to the ht value vector
            return res;
        }
        while (auto ret = upstream2->next()) {
            auto key = getKey(ret.value);
            if (ht.count(key)) {
                for (auto t : ht[key]) {
                    //build a tuple for every combination
                    intermediateTuples.push_back(buildResult(key, t, getValue2(ret.value)));
                }
                return next();
            }
        }
        return {};
    }

    void INLINE open() {
        upstream1->open();
        upstream2->open();
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
    vector<Tuple> intermediateTuples;

    //inline, pass as reference
    template<class UpstreamTuple>
    static constexpr KeyType getKey(UpstreamTuple t) {
        return *(const_cast<KeyType *>((KeyType *) (&t)));
    }

    template<class UpstreamTuple>
    static constexpr ValueType1 getValue1(UpstreamTuple t) {
        return *((ValueType1 *) (((char *) &t) + sizeof(KeyType)));
    }

    template<class UpstreamTuple>
    static constexpr ValueType2 getValue2(UpstreamTuple t) {
        return *((ValueType2 *) (((char *) &t) + sizeof(KeyType)));
    }

    Tuple buildResult(KeyType key, ValueType1 val1, ValueType2 val2) {
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
makeJoinOperator(Upstream1 *upstream1, Upstream2 *upstream2) {
    return JoinOperator<Upstream1, Upstream2, Tuple, KeyType, ValueType1, ValueType2>(upstream1, upstream2);
};


#endif //CPP_JOINOPERATOR_H
