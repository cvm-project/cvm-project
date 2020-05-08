#ifndef CODE_GEN_OPERATORS_JOINOPERATOR_H
#define CODE_GEN_OPERATORS_JOINOPERATOR_H

#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

/**
 * For input tuples (K, V) and (K, W) returns (K, V, W)
 * Returns every combination in case of repeating keys
 *
 * Current implementation builds map on the left input operator
 * Keeps the right operator input ordered
 */
template <class LeftUpstream, class RightUpstream, class Tuple, class KeyType,
          class LeftValueType, class RightValueType, size_t kNumKeys>
class JoinOperator {
public:
    JoinOperator(LeftUpstream *left_upstream, RightUpstream *right_upstream)
        : left_upstream_(left_upstream), right_upstream_(right_upstream){};

    void INLINE open() {
        left_upstream_->open();
        right_upstream_->open();

        // Build table already built
        if (!build_table_.empty()) {
            return;
        }

        // Build hash table from left upstream
        while (auto const ret = left_upstream_->next()) {
            auto const tuple = TupleToStdTuple(ret.value());
            auto const [key, value] = SplitTupleParameter<kNumKeys>(tuple);
            auto const [it, _] =
                    build_table_.insert({StdTupleToTuple(key), {}});
            it->second.emplace_back(StdTupleToTuple(value));
        }

        // Reset iterators of left matches
        left_matches_it_ = left_matches_end_;
    }

    Optional<Tuple> INLINE next() {
        // If there are no matches from the left upstream left to produce
        // results, we need a new tuple from the right upstream
        while (left_matches_it_ == left_matches_end_) {
            const auto ret = right_upstream_->next();
            if (!ret) return {};

            auto const tuple = TupleToStdTuple(ret.value());
            auto const [key_tuple, value_tuple] =
                    SplitTupleParameter<kNumKeys>(tuple);
            auto const key = StdTupleToTuple(key_tuple);
            auto const value = StdTupleToTuple(value_tuple);

            const auto it = build_table_.find(key);
            if (it != build_table_.end()) {
                left_matches_it_ = it->second.begin();
                left_matches_end_ = it->second.end();
                last_right_upstream_ = value;
                last_key_ = key;
            }
        }

        // At this point, we have a tuple from the right side with at least one
        // match on the left --> produce the next result

        auto const left_value = *left_matches_it_;
        left_matches_it_++;

        // Concatenate fields using std::tuple
        auto const key_tuple = TupleToStdTuple(last_key_);
        auto const left_tuple = TupleToStdTuple(left_value);
        auto const right_tuple = TupleToStdTuple(last_right_upstream_);
        auto const ret_tuple =
                std::tuple_cat(key_tuple, left_tuple, right_tuple);

        return StdTupleToTuple(ret_tuple);
    }

    void INLINE close() {
        left_upstream_->close();
        right_upstream_->close();
    }

private:
    template <std::size_t kN, std::size_t... kIs, std::size_t... kNs,
              typename... Ts>
    auto SplitTupleParameterImpl(std::index_sequence<kIs...>,
                                 std::index_sequence<kNs...>,
                                 std::tuple<Ts...> t) {
        return std::make_pair(std::make_tuple(std::get<kIs>(t)...),
                              std::make_tuple(std::get<kNs + kN>(t)...));
    }

    template <std::size_t kN, typename... Ts>
    auto SplitTupleParameter(std::tuple<Ts...> t) {
        return SplitTupleParameterImpl<kN>(
                std::make_index_sequence<kN>(),
                std::make_index_sequence<sizeof...(Ts) - kN>(), t);
    }

    struct KeyTypeHash {
    private:
        template <std::size_t... kIs, typename... Ts>
        auto CallOperatorImpl(std::index_sequence<kIs...>,
                              std::tuple<Ts...> t) const {
            return (... ^ std::hash<Ts>()(std::get<kIs>(t)));
        }

    public:
        size_t operator()(const KeyType &key) const {
            auto key_tuple = TupleToStdTuple(key);
            return CallOperatorImpl(std::make_index_sequence<kNumKeys>(),
                                    key_tuple);
        }
    };

    struct KeyTypeEquals {
        bool operator()(const KeyType &lhs, const KeyType &rhs) const {
            return TupleToStdTuple(lhs) == TupleToStdTuple(rhs);
        }
    };

    LeftUpstream *const left_upstream_;
    RightUpstream *const right_upstream_;
    std::unordered_map<KeyType, std::vector<LeftValueType>, KeyTypeHash,
                       KeyTypeEquals>
            build_table_;
    RightValueType last_right_upstream_;
    KeyType last_key_;
    typename std::vector<LeftValueType>::iterator left_matches_it_;
    typename std::vector<LeftValueType>::iterator left_matches_end_;
};

template <class Tuple, class KeyType, class LeftValueType, class RightValueType,
          size_t kNumKeys, class LeftUpstream, class RightUpstream>
JoinOperator<LeftUpstream, RightUpstream, Tuple, KeyType, LeftValueType,
             RightValueType, kNumKeys>
        INLINE makeJoinOperator(LeftUpstream *left_upstream,
                                RightUpstream *right_upstream) {
    return JoinOperator<LeftUpstream, RightUpstream, Tuple, KeyType,
                        LeftValueType, RightValueType, kNumKeys>(
            left_upstream, right_upstream);
};

#endif  // CODE_GEN_OPERATORS_JOINOPERATOR_H
