//
// Created by dan on 14.11.19.
//

#ifndef CODE_GEN_OPERATORS_REDUCEBYINDEXOPERATOR_H
#define CODE_GEN_OPERATORS_REDUCEBYINDEXOPERATOR_H

#include <iostream>
#include <limits>
#include <vector>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

/**
 * Groups up the input tuples by an integer key in a vector and reduces the vals
 * Binary function must be associative, commutative
 * The return type of the function must be the same as its arguments
 *
 * Result tuples are in an arbitrary order
 */
template <class Upstream, class Tuple, class KeyType, class ValueType,
          class Function>
class ReduceByIndexOperator {
    const int64_t lo_;
    const int64_t hi_;

    std::vector<ValueType> inner_vector_;
    std::vector<bool> presence_vector_;

    typename std::vector<ValueType>::iterator iter_values_;
    typename std::vector<bool>::iterator iter_presence_;

public:
    Upstream *const upstream_;
    Function func_;

    ReduceByIndexOperator(Upstream *upstream, Function func, int64_t lo,
                          int64_t hi)
        : upstream_(upstream),
          func_(func),
          lo_(lo),
          hi_(hi),
          inner_vector_(hi - lo + 1),
          presence_vector_(hi - lo + 1, false){};

    Optional<Tuple> INLINE next() {
        // Find the first non-NULL element
        while (iter_values_ != inner_vector_.end()) {
            if (*iter_presence_) {
                const auto res = TupleToStdTuple(*iter_values_);
                const auto old_key = std::make_tuple(
                        iter_values_ - inner_vector_.begin() + lo_);

                ++iter_presence_;
                ++iter_values_;

                return StdTupleToTuple(std::tuple_cat(old_key, res));
            }

            ++iter_values_;
            ++iter_presence_;
        }

        return {};
    }

    void INLINE open() {
        upstream_->open();
        while (const auto ret = upstream_->next()) {
            const auto tuple = TupleToStdTuple(ret.value());
            const auto [key_tuple, value_tuple] = SplitTuple(tuple);
            const auto key = StdTupleToTuple(key_tuple).v0 - lo_;
            const auto val = StdTupleToTuple(value_tuple);

            // Further compute the result, and mark this entry as present
            inner_vector_.at(key) = presence_vector_.at(key)
                                            ? func_(inner_vector_.at(key), val)
                                            : val;
            presence_vector_.at(key) = true;
        }
        iter_values_ = inner_vector_.begin();
        iter_presence_ = presence_vector_.begin();
    }

    void INLINE close() { upstream_->close(); }
};

template <class Tuple, class KeyType, class ValueType, int64_t lo, int64_t hi,
          class Upstream, class Function>
ReduceByIndexOperator<Upstream, Tuple, KeyType, ValueType, Function> INLINE
makeReduceByIndexOperator(Upstream *upstream, Function func) {
    using KeyElement = typename std::remove_reference<decltype(
            std::declval<KeyType>().v0)>::type;

    static_assert(std::numeric_limits<KeyElement>::is_integer,
                  "The keys  must have an integer type.");

    return ReduceByIndexOperator<Upstream, Tuple, KeyType, ValueType, Function>(
            upstream, func, lo, hi);
};

#endif  // CODE_GEN_OPERATORS_REDUCEBYINDEXOPERATOR_H
