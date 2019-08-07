#ifndef CPP_SPLITCOLUMNDATAOPERATOR_H
#define CPP_SPLITCOLUMNDATAOPERATOR_H

#include <utility>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class Upstream, class Tuple>
class SplitColumnDataOperator {
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

    template <class StdTuple, std::size_t... I>
    static auto MakeColumnSlicesImpl(const StdTuple &tuple,
                                     const size_t outer_shape,
                                     const size_t offset, const size_t shape,
                                     std::index_sequence<I...>) {
        assert(((std::get<I>(tuple).shape[0] == std::get<0>(tuple).shape[0]) &&
                ...));
        return std::make_tuple(decltype(std::get<I>(tuple)){
                std::get<I>(tuple).data, outer_shape, offset, shape}...);
    }

    template <class StdTuple>
    static auto MakeColumnSlices(const StdTuple &tuple,
                                 const size_t outer_shape, const size_t offset,
                                 const size_t shape) {
        return MakeColumnSlicesImpl(
                tuple, outer_shape, offset, shape,
                std::make_index_sequence<std::tuple_size<StdTuple>::value>());
    }

public:
    SplitColumnDataOperator(Upstream *const upstream, const size_t num_slices)
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

        auto const tuple = TupleToStdTuple(current_tuple_);

        // Compute indices
        const auto outer_shape = std::get<0>(tuple).outer_shape[0];
        const auto offset = std::get<0>(tuple).offsets[0];
        const auto size = std::get<0>(tuple).shape[0];

        const auto first_idx =
                ComputeFirstIndex(size, current_slice_num_, num_slices_);
        const auto last_idx =
                ComputeLastIndex(size, current_slice_num_, num_slices_);

        current_slice_num_++;

        // Extract column slices
        const auto data = current_tuple_.v0.data;

        const auto ret = MakeColumnSlices(
                tuple, outer_shape, offset + first_idx, last_idx - first_idx);

        return StdTupleToTuple(ret);
    }

    void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    const size_t num_slices_;
    Tuple current_tuple_{};
    size_t current_slice_num_{};
};

template <class Tuple, class Upstream>
SplitColumnDataOperator<Upstream, Tuple> makeSplitColumnDataOperator(
        Upstream *const upstream, const size_t num_slices) {
    return SplitColumnDataOperator<Upstream, Tuple>(upstream, num_slices);
};

#endif  // CPP_SPLITCOLUMNDATAOPERATOR_H
