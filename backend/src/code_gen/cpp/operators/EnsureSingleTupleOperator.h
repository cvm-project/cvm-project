#ifndef CPP_ENSURESINGLETUPLE_OPERATOR_H
#define CPP_ENSURESINGLETUPLE_OPERATOR_H

#include "Optional.h"
#include "Utils.h"

#include <stdexcept>

template <class Upstream, class Tuple>
class EnsureSingleTupleOperator {
public:
    EnsureSingleTupleOperator(Upstream *upstream) : upstream_(upstream) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<Tuple> next() {
        const auto ret = upstream_->next();
        if (!ret) {
            return {};
        }
        if (upstream_->next()) {
            throw std::runtime_error(
                    "Found several tuples where a single one was expected");
        }
        return ret.value();
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream *upstream_;
};

template <class Tuple, class Upstream>
EnsureSingleTupleOperator<Upstream, Tuple> makeEnsureSingleTupleOperator(
        Upstream *upstream) {
    return EnsureSingleTupleOperator<Upstream, Tuple>(upstream);
};

#endif  // CPP_ENSURESINGLETUPLE_OPERATOR_H
