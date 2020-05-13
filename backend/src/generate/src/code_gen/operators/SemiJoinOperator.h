#ifndef CODE_GEN_OPERATORS_SEMIJOINOPERATOR_H
#define CODE_GEN_OPERATORS_SEMIJOINOPERATOR_H

#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

/**
 * For input tuples (K, V) and (K, W) returns (K, V)
 *
 * Current implementation builds map on the right input operator
 * Keeps the left operator input ordered
 */
template <class LeftUpstream, class RightUpstream, class Tuple, class KeyType,
          class LeftValueType>
class SemiJoinOperator {
public:
    SemiJoinOperator(LeftUpstream *left_upstream, RightUpstream *right_upstream)
        : left_upstream_(left_upstream), right_upstream_(right_upstream){};

    void INLINE open() {
        left_upstream_->open();
        right_upstream_->open();

        // Build table already built
        if (!build_table_.empty()) {
            return;
        }

        // Build hash table from right upstream
        while (auto const ret = right_upstream_->next()) {
            auto const tuple = TupleToStdTuple(ret.value());
            auto const [key, value] = SplitTuple(tuple);
            build_table_.insert({StdTupleToTuple(key)});
        }
    }

    Optional<Tuple> INLINE next() {
        while (auto ret = left_upstream_->next()) {
            auto const tuple = TupleToStdTuple(ret.value());
            auto const [key_tuple, _] = SplitTuple(tuple);
            auto const key = StdTupleToTuple(key_tuple);
            const auto it = build_table_.find(key);
            if (it != build_table_.end()) {
                return ret.value();
            }
        }

        return {};
    }

    void INLINE close() {
        left_upstream_->close();
        right_upstream_->close();
    }

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

    LeftUpstream *const left_upstream_;
    RightUpstream *const right_upstream_;
    std::unordered_set<KeyType, KeyTypeHash, KeyTypeEquals> build_table_;
    LeftValueType last_left_upstream_;
    KeyType last_key_;
};

template <class Tuple, class KeyType, class LeftValueType, class LeftUpstream,
          class RightUpstream>
SemiJoinOperator<LeftUpstream, RightUpstream, Tuple, KeyType, LeftValueType>
        INLINE makeSemiJoinOperator(LeftUpstream *left_upstream,
                                    RightUpstream *right_upstream) {
    return SemiJoinOperator<LeftUpstream, RightUpstream, Tuple, KeyType,
                            LeftValueType>(left_upstream, right_upstream);
};

#endif  // CODE_GEN_OPERATORS_SEMIJOINOPERATOR_H
