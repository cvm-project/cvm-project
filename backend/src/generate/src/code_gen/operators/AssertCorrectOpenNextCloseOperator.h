#ifndef CPP_ASSERTCORRECTOPENNEXTCLOSEOPERATOR_H
#define CPP_ASSERTCORRECTOPENNEXTCLOSEOPERATOR_H

#include <cassert>

#include "Utils.h"
#include "runtime/operators/optional.hpp"

template <class Upstream, class Tuple>
class AssertCorrectOpenNextCloseOperator {
public:
    AssertCorrectOpenNextCloseOperator(Upstream *const upstream)
        : upstream_(upstream), is_open_(false) {}

    ~AssertCorrectOpenNextCloseOperator() { assert(!is_open_); }

    INLINE void open() {
        assert(!is_open_);
        is_open_ = true;
        upstream_->open();
    }

    INLINE Optional<Tuple> next() {
        assert(is_open_);
        return upstream_->next();
    }

    INLINE void close() {
        assert(is_open_);
        is_open_ = false;
        upstream_->close();
    }

private:
    Upstream *const upstream_;
    bool is_open_;
};

template <class Tuple, class Upstream>
AssertCorrectOpenNextCloseOperator<Upstream, Tuple>
makeAssertCorrectOpenNextCloseOperator(Upstream *const upstream) {
    return AssertCorrectOpenNextCloseOperator<Upstream, Tuple>(upstream);
};

#endif  // CPP_ASSERTCORRECTOPENNEXTCLOSEOPERATOR_H
