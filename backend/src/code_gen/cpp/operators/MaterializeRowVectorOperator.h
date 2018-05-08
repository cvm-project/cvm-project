#ifndef CPP_MATERIALIZEROWVECTOROPERATOR_H
#define CPP_MATERIALIZEROWVECTOROPERATOR_H

#include <cstdlib>

#include "Operator.h"

template <class OuterTuple, class InnerTuple, class Upstream>
class MaterializeRowVectorOperator : public Operator {
public:
    MaterializeRowVectorOperator(Upstream *upstream)
        : upstream(upstream), result_ptr(nullptr), result_size(0) {}

    INLINE void open() { upstream->open(); }

    INLINE Optional<OuterTuple> next() {
        if (result_ptr == nullptr) materialize_upstream();
        return OuterTuple{result_ptr, result_size};
    }

    INLINE void close() {}

private:
    void materialize_upstream() {
        size_t allocated_size = 1;
        result_ptr = reinterpret_cast<InnerTuple *>(
                malloc(sizeof(InnerTuple) * allocated_size));
        while (auto tuple = upstream->next()) {
            if (allocated_size <= result_size) {
                allocated_size *= 2;
                result_ptr = reinterpret_cast<InnerTuple *>(realloc(
                        result_ptr, sizeof(InnerTuple) * allocated_size));
                if (result_ptr == nullptr) throw "out of memory";
            }
            result_ptr[result_size] = tuple.value;
            result_size++;
        }
        upstream->close();
    }

    Upstream *const upstream;
    InnerTuple *result_ptr;
    size_t result_size;
};

template <class OuterTuple, class InnerTuple, class Upstream>
MaterializeRowVectorOperator<OuterTuple, InnerTuple, Upstream>
makeMaterializeRowVectorOperator(Upstream *upstream) {
    return MaterializeRowVectorOperator<OuterTuple, InnerTuple, Upstream>(
            upstream);
};

#endif  // CPP_MATERIALIZEROWVECTOROPERATOR_H
