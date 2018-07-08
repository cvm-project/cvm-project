#ifndef CPP_MATERIALIZEROWVECTOROPERATOR_H
#define CPP_MATERIALIZEROWVECTOROPERATOR_H

#include <type_traits>

#include "Operator.h"

template <typename T, typename U>
auto constexpr max(T x, U y) ->
        typename std::remove_reference<decltype(x > y ? x : y)>::type {
    return x > y ? x : y;
}

template <class OuterTuple, class InnerTuple, class Upstream>
class MaterializeRowVectorOperator : public Operator {
public:
    MaterializeRowVectorOperator(Upstream *const upstream)
        : upstream_(upstream), result_ptr_(nullptr), result_size_(0) {}

    INLINE void open() {}

    INLINE Optional<OuterTuple> next() {
        if (result_ptr_ == nullptr) MaterializeUpstream();
        return OuterTuple{result_ptr_, result_size_};
    }

    INLINE void close() {}

private:
    void MaterializeUpstream() {
        upstream_->open();

        size_t allocated_size = 1;
        result_ptr_ = reinterpret_cast<InnerTuple *>(
                malloc(sizeof(InnerTuple) * allocated_size));
        while (auto tuple = upstream_->next()) {
            if (allocated_size <= result_size_) {
                allocated_size *= 2;
                result_ptr_ = reinterpret_cast<InnerTuple *>(realloc(
                        result_ptr_, sizeof(InnerTuple) * allocated_size));
                if (result_ptr_ == nullptr)
                    throw std::runtime_error(
                            "Could not materialize: out of memory.");
            }
            result_ptr_[result_size_] = tuple.value;
            result_size_++;
        }

        result_ptr_ = reinterpret_cast<InnerTuple *>(
                realloc(result_ptr_,
                        sizeof(InnerTuple) * max(size_t(1), result_size_)));

        upstream_->close();
    }

    Upstream *const upstream_;
    InnerTuple *result_ptr_;
    size_t result_size_;
};

template <class OuterTuple, class InnerTuple, class Upstream>
MaterializeRowVectorOperator<OuterTuple, InnerTuple, Upstream>
makeMaterializeRowVectorOperator(Upstream *upstream) {
    return MaterializeRowVectorOperator<OuterTuple, InnerTuple, Upstream>(
            upstream);
};

#endif  // CPP_MATERIALIZEROWVECTOROPERATOR_H
