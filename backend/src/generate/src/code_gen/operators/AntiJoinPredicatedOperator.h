#ifndef CODE_GEN_OPERATORS_ANTIJOINOPERATOR_PREDICATED_H
#define CODE_GEN_OPERATORS_ANTIJOINOPERATOR_PREDICATED_H

#include <tuple>
#include <unordered_map>
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
          class LeftValueType, class RightValueType, class Function>
class AntiJoinPredicatedOperator {
public:
    AntiJoinPredicatedOperator(LeftUpstream *left_upstream,
                               RightUpstream *right_upstream, Function func)
        : left_upstream_(left_upstream),
          right_upstream_(right_upstream),
          function(func){};

    Function function;

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
            auto const [it, _] =
                    build_table_.insert({StdTupleToTuple(key), {}});
            it->second.emplace_back(StdTupleToTuple(value));
        }
    }

    Optional<Tuple> INLINE next() {
        while (auto ret = left_upstream_->next()) {
            auto const tuple = TupleToStdTuple(ret.value());
            auto const [key_tuple, value_tuple] = SplitTuple(tuple);
            auto const key = StdTupleToTuple(key_tuple);
            auto const value = StdTupleToTuple(value_tuple);
            const auto it = build_table_.find(key);
            if (it == build_table_.end()) {
                return ret.value();
            } else {
                bool res = false;
                auto itv = it->second.begin();
                while (!res and itv != it->second.end()) {
                    auto ret_tuple =
                            std::tuple_cat(key_tuple, TupleToStdTuple(*itv));
                    res = res or function(ret, StdTupleToTuple(ret_tuple));
                    itv++;
                }
                if (!res and !it->second.empty()) {
                    return ret.value();
                }
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
    std::unordered_map<KeyType, std::vector<RightValueType>, KeyTypeHash,
                       KeyTypeEquals>
            build_table_;
    LeftValueType last_left_upstream_;
    KeyType last_key_;
};

template <class Tuple, class KeyType, class LeftValueType, class RightValueType,
          class LeftUpstream, class RightUpstream, class Function>
AntiJoinPredicatedOperator<LeftUpstream, RightUpstream, Tuple, KeyType,
                           LeftValueType, RightValueType, Function>
        INLINE makeAntiJoinPredicatedOperator(LeftUpstream *left_upstream,
                                              RightUpstream *right_upstream,
                                              Function func) {
    return AntiJoinPredicatedOperator<LeftUpstream, RightUpstream, Tuple,
                                      KeyType, LeftValueType, RightValueType,
                                      Function>(left_upstream, right_upstream,
                                                func);
};

#endif  // CODE_GEN_OPERATORS_ANTIJOINOPERATOR_PREDICATED_H
