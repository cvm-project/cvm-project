#ifndef CODE_GEN_OPERATORS_REDUCEBYKEYOPERATOR_H
#define CODE_GEN_OPERATORS_REDUCEBYKEYOPERATOR_H

#include <iostream>
#include <unordered_map>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

/**
 * Groups up the input tuples by key and reduces the values
 * Binary function must be associative, commutative
 * The return type of the function must be the same as its arguments
 *
 * Result tuples are in an arbitrary order
 *
 */
template <class Upstream, class Tuple, class KeyType, class ValueType,
          class Function>
class ReduceByKeyOperator {
public:
    ReduceByKeyOperator(Upstream *const upstream, const Function &func)
        : upstream_(upstream), func_(func){};

    Optional<Tuple> INLINE next() {
        if (current_result_it_ == hash_table_.end()) {
            return {};
        }
        const auto &res = *(current_result_it_++);
        const auto key_tuple = TupleToStdTuple(res.first);
        const auto value_tuple = TupleToStdTuple(res.second);
        return StdTupleToTuple(std::tuple_cat(key_tuple, value_tuple));
    }

    void INLINE open() {
        upstream_->open();
        while (const auto ret = upstream_->next()) {
            auto const tuple = TupleToStdTuple(ret.value());
            auto const [key_tuple, value_tuple] = SplitTuple(tuple);
            auto const key = StdTupleToTuple(key_tuple);
            auto const value = StdTupleToTuple(value_tuple);

            const auto [it, is_new] = hash_table_.emplace(key, value);
            if (!is_new) {
                const auto &aggregate = it->second;
                it->second = func_(aggregate, value);
            }
        }
        current_result_it_ = hash_table_.begin();
    }

    void INLINE close() { upstream_->close(); }

private:
    struct KeyTypeHash {
        size_t operator()(const KeyType &key) const {
            using FieldType = std::remove_cv_t<decltype(key.v0)>;
            return std::hash<FieldType>()(key.v0);
        }
    };

    struct KeyTypeEquals {
        bool operator()(const KeyType &lhs, const KeyType &rhs) const {
            return TupleToStdTuple(lhs) == TupleToStdTuple(rhs);
        }
    };

    Upstream *const upstream_;
    Function func_;
    std::unordered_map<KeyType, ValueType, KeyTypeHash, KeyTypeEquals>
            hash_table_;
    typename decltype(hash_table_)::iterator current_result_it_;
};

template <class Tuple, class KeyType, class ValueType, class Upstream,
          class Function>
ReduceByKeyOperator<Upstream, Tuple, KeyType, ValueType, Function> INLINE
makeReduceByKeyOperator(Upstream *upstream, Function func) {
    return ReduceByKeyOperator<Upstream, Tuple, KeyType, ValueType, Function>(
            upstream, func);
};

#endif  // CODE_GEN_OPERATORS_REDUCEBYKEYOPERATOR_H
