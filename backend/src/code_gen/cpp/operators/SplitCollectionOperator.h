#ifndef CPP_SPLITCOLLECTIONOPERATOR_H
#define CPP_SPLITCOLLECTIONOPERATOR_H

#include "Optional.h"
#include "Utils.h"

template <class Upstream, class Tuple>
class SplitCollectionOperator {
public:
    typedef decltype(Tuple().v0) ValueType;

private:
    static constexpr size_t ComputeFirstIndex(const size_t size,
                                              const size_t slice_num,
                                              const size_t num_slices) {
        return slice_num * size / num_slices;
    }

    static constexpr size_t ComputeLastIndex(const size_t size,
                                             const size_t slice_num,
                                             const size_t num_slices) {
        return ComputeFirstIndex(size, slice_num + 1, num_slices);
    }

public:
    SplitCollectionOperator(Upstream *const upstream, const size_t num_slices)
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

        const auto data = current_tuple_.v0.data;
        const auto outer_shape = current_tuple_.v0.outer_shape[0];
        const auto offset = current_tuple_.v0.offsets[0];
        const auto size = current_tuple_.v0.shape[0];

        const auto first_idx =
                ComputeFirstIndex(size, current_slice_num_, num_slices_);
        const auto last_idx =
                ComputeLastIndex(size, current_slice_num_, num_slices_);

        current_slice_num_++;

        return Tuple{data, outer_shape, offset + first_idx,
                     last_idx - first_idx};
    }

    void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    const size_t num_slices_;
    Tuple current_tuple_{};
    size_t current_slice_num_{};
};

template <class Tuple, class Upstream>
SplitCollectionOperator<Upstream, Tuple> makeSplitCollectionOperator(
        Upstream *const upstream, const size_t num_slices) {
    return SplitCollectionOperator<Upstream, Tuple>(upstream, num_slices);
};

#endif  // CPP_SPLITCOLLECTIONOPERATOR_H
