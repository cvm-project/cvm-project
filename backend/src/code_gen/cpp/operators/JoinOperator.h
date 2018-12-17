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

    Optional<Tuple> INLINE next() {
        if (intermediate_tuples_ != iterator_end_) {
            auto res = *intermediate_tuples_;
            intermediate_tuples_++;
            // build result tuple here
            // from iterator to the ht_ value vector
            return BuildResult(last_key_, res, last_right_upstream_);
        }
        while (auto ret = right_upstream_->next()) {
            auto key = ExtractKey(ret.value());
            if (ht_.count(key)) {
                intermediate_tuples_ = ht_[key].begin();
                iterator_end_ = ht_[key].end();
                last_right_upstream_ = ExtractRightValue(ret.value());
                last_key_ = key;
                return next();
            }
        }
        return {};
    }

    void INLINE open() {
        left_upstream_->open();
        right_upstream_->open();
        if (!ht_.empty()) {
            return;
        }
        while (auto ret = left_upstream_->next()) {
            if (ht_.count(ExtractKey(ret.value())) > 0) {
                ht_[ExtractKey(ret.value())].push_back(
                        ExtractLeftValue(ret.value()));
            } else {
                std::vector<LeftValueType> values;
                values.push_back(ExtractLeftValue(ret.value()));
                ht_.emplace(ExtractKey(ret.value()), values);
            }
        }
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

    INLINE static Tuple BuildResult(const KeyType &key,
                                    const LeftValueType &left_val,
                                    const RightValueType &right_val) {
        auto const left_tuple = TupleToStdTuple(left_val);
        auto const right_tuple = TupleToStdTuple(right_val);
        auto const key_tuple = TupleToStdTuple(key);
        auto const ret_tuple =
                std::tuple_cat(key_tuple, left_tuple, right_tuple);
        return StdTupleToTuple(ret_tuple);
    }

    LeftUpstream *const left_upstream_;
    RightUpstream *const right_upstream_;
    std::unordered_map<KeyType, std::vector<LeftValueType>, hash, pred> ht_;
    RightValueType last_right_upstream_;
    KeyType last_key_;
    typename std::vector<LeftValueType>::iterator intermediate_tuples_;
    typename std::vector<LeftValueType>::iterator iterator_end_;
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
