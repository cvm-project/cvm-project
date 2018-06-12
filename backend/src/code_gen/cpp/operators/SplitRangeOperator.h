#ifndef CPP_SPLITRANGEOPERATOR_H
#define CPP_SPLITRANGEOPERATOR_H

#include <omp.h>

#include "Operator.h"

template <class Upstream, class Tuple>
class SplitRangeOperator : public Operator {
public:
    typedef decltype(Tuple().v0) ValueType;

private:
    static constexpr ValueType ComputeStartIndex(const ValueType start,
                                                 const ValueType end,
                                                 const ValueType step,
                                                 const size_t slice_num,
                                                 const size_t num_slices) {
        return start +
               (slice_num * size_t((end - start) / step) / num_slices) * step;
    }

    static constexpr ValueType ComputeEndIndex(const ValueType start,
                                               const ValueType end,
                                               const ValueType step,
                                               const size_t slice_num,
                                               const size_t num_slices) {
        if (slice_num + 1 == num_slices) return end;
        return ComputeStartIndex(start, end, step, slice_num + 1, num_slices);
    }

public:
    SplitRangeOperator(Upstream *const upstream, const size_t num_slices)
        : upstream_(upstream), num_slices_(num_slices) {}

    void open() {
        upstream_->open();
        current_slice_num_ = num_slices_;
    }

    INLINE Optional<Tuple> next() {
        if (current_slice_num_ == num_slices_) {
            const auto ret = upstream_->next();
            if (!ret) return {};

            current_tuple_ = ret.value();
            current_slice_num_ = 0;
        }

        const auto from = current_tuple_.v0;
        const auto to = current_tuple_.v1;
        const auto step = current_tuple_.v2;

        const auto ret =
                Tuple{ComputeStartIndex(from, to, step, current_slice_num_,
                                        num_slices_),
                      ComputeEndIndex(from, to, step, current_slice_num_,
                                      num_slices_),
                      step};

        current_slice_num_++;
        return ret;
    }

    void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    const size_t num_slices_;
    Tuple current_tuple_{};
    size_t current_slice_num_{};
};

template <class Tuple, class Upstream>
SplitRangeOperator<Upstream, Tuple> makeSplitRangeOperator(
        Upstream *const upstream, const size_t num_slices) {
    return SplitRangeOperator<Upstream, Tuple>(upstream, num_slices);
};

#endif  // CPP_SPLITRANGEOPERATOR_H
