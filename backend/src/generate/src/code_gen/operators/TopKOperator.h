#ifndef CODE_GEN_OPERATORS_TOPK_H
#define CODE_GEN_OPERATORS_TOPK_H

#include <set>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class Upstream, class Tuple>
class TopKOperator {
public:
    TopKOperator(Upstream* const upstream, int64_t const num_elements)
        : upstream_(upstream), num_elements_(num_elements) {}

    void INLINE open() {
        upstream_->open();

        for (int i = 0; i < num_elements_; ++i) {
            if (auto const tuple = upstream_->next()) {
                container_.insert(tuple.value());
            }
        }
        Tuple smallest = *container_.rbegin();

        while (auto tuple = upstream_->next()) {
            if (tuple.value().v0 > smallest.v0) {
                container_.erase(--container_.end());
                container_.insert(tuple.value());
                smallest = *container_.rbegin();
            }
        }
        result_it_ = container_.begin();
    }

    Optional<Tuple> INLINE next() {
        if (result_it_ != container_.end()) {
            return *result_it_++;
        }
        return {};
    }

    void INLINE close() { upstream_->close(); }

private:
    struct GreatestComparator {
        bool operator()(const Tuple& x, const Tuple& y) const {
            return x.v0 > y.v0;
        }
    };

    Upstream* const upstream_;
    int64_t num_elements_;
    std::multiset<Tuple, GreatestComparator> container_;
    typename decltype(container_)::iterator result_it_{};
};

template <class Tuple, class Upstream>
TopKOperator<Upstream, Tuple> makeTopKOperator(Upstream* upstream,
                                               const int64_t num_elements) {
    return TopKOperator<Upstream, Tuple>(upstream, num_elements);
};

#endif  // CODE_GEN_OPERATORS_REDUCEBYINDEXOPERATOR_H
