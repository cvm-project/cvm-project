//
// Created by sabir on 04.07.17.
//

#ifndef CPP_JOINOPERATOR_H
#define CPP_JOINOPERATOR_H

#include <tuple>
#include <unordered_map>
#include <vector>

#include "Optional.h"
#include "Utils.h"

/**
 * For input tuples (K, V) and (K, W) returns (K, V, W)
 * Returns every combination in case of repeating keys
 *
 * Current implementation builds map on the left input operator
 * Keeps the right operator input ordered
 */
template <class LeftUpstream, class RightUpstream, class Tuple, class KeyType,
          class LeftValueType, class RightValueType>
class JoinOperator {
public:
    JoinOperator(LeftUpstream *left_upstream, RightUpstream *right_upstream)
        : left_upstream_(left_upstream), right_upstream_(right_upstream){};

    void INLINE open() {
        left_upstream_->open();
        right_upstream_->open();

        // Build table already built
        if (!ht_.empty()) {
            return;
        }

        // Build hash table from left upstream
        while (auto ret = left_upstream_->next()) {
            auto const key = ExtractKey(ret.value());
            auto const left_value = ExtractLeftValue(ret.value());

            auto const[it, _] = ht_.insert({key, {}});
            it->second.emplace_back(left_value);
        }

        // Reset iterators of left matches
        left_matches_it_ = left_matches_end_;
    }

    Optional<Tuple> INLINE next() {
        // If there are no matches from the left upstream left to produces
        // results, we need a new tuple from the right upstream
        while (left_matches_it_ == left_matches_end_) {
            const auto ret = right_upstream_->next();
            if (!ret) return {};

            const auto key = ExtractKey(ret.value());
            const auto it = ht_.find(key);
            if (it != ht_.end()) {
                left_matches_it_ = it->second.begin();
                left_matches_end_ = it->second.end();
                last_right_upstream_ = ExtractRightValue(ret.value());
                last_key_ = key;
            }
        }

        // At this point, we have a tuple from the right side with at least one
        // match on the left --> produce the next result

        auto const left_value = *left_matches_it_;
        left_matches_it_++;

        // Concate fields using std::tuple
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
    struct hash {
        size_t operator()(const KeyType x) const {
            return std::hash<long>()(*((long *)(&x)));
        }
    };

    struct pred {
        bool operator()(const KeyType x, const KeyType y) const {
            return *((long *)(&x)) == *((long *)(&y));
        }
    };

    template <class UpstreamTuple>
    INLINE static constexpr KeyType ExtractKey(const UpstreamTuple &t) {
        return *(const_cast<KeyType *>((KeyType *)(&t)));
    }

    template <class UpstreamTuple>
    INLINE static constexpr LeftValueType ExtractLeftValue(
            const UpstreamTuple &t) {
        return *((LeftValueType *)(((char *)&t) + sizeof(KeyType)));
    }

    template <class UpstreamTuple>
    INLINE static constexpr RightValueType ExtractRightValue(
            const UpstreamTuple &t) {
        return *((RightValueType *)(((char *)&t) + sizeof(KeyType)));
    }

    LeftUpstream *const left_upstream_;
    RightUpstream *const right_upstream_;
    std::unordered_map<KeyType, std::vector<LeftValueType>, hash, pred> ht_;
    RightValueType last_right_upstream_;
    KeyType last_key_;
    typename std::vector<LeftValueType>::iterator left_matches_it_;
    typename std::vector<LeftValueType>::iterator left_matches_end_;
};

template <class Tuple, class KeyType, class LeftValueType, class RightValueType,
          class LeftUpstream, class RightUpstream>
JoinOperator<LeftUpstream, RightUpstream, Tuple, KeyType, LeftValueType,
             RightValueType>
        INLINE makeJoinOperator(LeftUpstream *left_upstream,
                                RightUpstream *right_upstream) {
    return JoinOperator<LeftUpstream, RightUpstream, Tuple, KeyType,
                        LeftValueType, RightValueType>(left_upstream,
                                                       right_upstream);
};

#endif  // CPP_JOINOPERATOR_H
