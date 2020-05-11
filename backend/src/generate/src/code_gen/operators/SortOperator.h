
#ifndef CODE_GEN_OPERATORS_SORTOPERATOR_H
#define CODE_GEN_OPERATORS_SORTOPERATOR_H

#include <algorithm>
#include <vector>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class Upstream, class Tuple>
class SortOperator {
public:
    SortOperator(Upstream* upstream) : upstream_(upstream) {}

    INLINE void open() {
        upstream_->open();
        while (auto const ret = upstream_->next()) {
            container_.push_back(ret.value());
        }
        std::sort(container_.begin(), container_.end(), SmallerComparator());
        result_it_ = container_.begin();
    }

    INLINE Optional<Tuple> next() {
        if (result_it_ != container_.end()) {
            return *result_it_++;
        }
        return {};
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream* const upstream_;
    std::vector<Tuple> container_;
    typename decltype(container_)::iterator result_it_;

    struct SmallerComparator {
        bool operator()(const Tuple& x, const Tuple& y) const {
            return x.v0 < y.v0;
        }
    };
};

template <class Tuple, class Upstream>
SortOperator<Upstream, Tuple> makeSortOperator(Upstream* upstream) {
    return SortOperator<Upstream, Tuple>(upstream);
};

#endif  // CODE_GEN_OPERATORS_SORTOPERATOR_H
