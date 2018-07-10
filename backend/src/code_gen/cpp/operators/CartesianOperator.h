//
// Created by sabir on 04.07.17.
//

#ifndef CPP_CARTESIANOPERATOR_H
#define CPP_CARTESIANOPERATOR_H

#include <vector>

#include "Operator.h"

/**
 * Assumes the left operator is the smaller relation
 * Keeps the right operator input ordered
 *
 */
template <class Upstream1, class Upstream2, class Tuple, class UpstreamTuple>
class CartesianOperator : public Operator {
public:
    Upstream1 *upstream1;
    Upstream2 *upstream2;

    CartesianOperator(Upstream1 *upstream1, Upstream2 *upstream2)
        : upstream1(upstream1), upstream2(upstream2){};

    Optional<Tuple> INLINE next() {
        // iterate over the smaller relation
        if (auto t1 = upstream1->next()) {
            // currenttuple could be empty
            if (currentTuple) {
                return buildResult(t1.value(), currentTuple.value());
            }
        }
        currentTuple = upstream2->next();
        if (currentTuple) {
            upstream1->close();
            upstream1->open();
            return next();
        }
        return {};
    }

    void INLINE open() {
        upstream1->open();
        upstream2->open();
        currentTuple = upstream2->next();
    }

    void INLINE close() {
        upstream1->close();
        upstream2->close();
    }

private:
    Optional<UpstreamTuple> currentTuple;

    template <class T1, class T2>
    INLINE static Tuple buildResult(const T1 &val1, const T2 &val2) {
        Tuple res;
        char *resp = (char *)&res;
        *((T1 *)resp) = val1;
        *((T2 *)(resp + sizeof(T1))) = val2;
        ;
        return res;
    }
};

template <class Tuple, class UpstreamTuple, class Upstream1, class Upstream2>
CartesianOperator<Upstream1, Upstream2, Tuple, UpstreamTuple> INLINE
makeCartesianOperator(Upstream1 *upstream1, Upstream2 *upstream2) {
    return CartesianOperator<Upstream1, Upstream2, Tuple, UpstreamTuple>(
            upstream1, upstream2);
};

#endif  // CPP_CARTESIANOPERATOR_H
