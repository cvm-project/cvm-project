#ifndef CODE_GEN_OPERATORS_SPLITCOLUMNDATAOPERATOR_H
#define CODE_GEN_OPERATORS_SPLITCOLUMNDATAOPERATOR_H

#include <utility>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class OutputTuple, class MainUpstream, class DopUpstream>
class SplitColumnDataOperator {
public:
    typedef decltype(OutputTuple().v0) ValueType;

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
    SplitColumnDataOperator(MainUpstream *const main_upstream,
                            DopUpstream *const dop_upstream)
        : main_upstream_(main_upstream), dop_upstream_(dop_upstream) {}

    void open() {
        dop_upstream_->open();
        num_slices_ = dop_upstream_->next().value().v0;
        assert(!dop_upstream_->next());
        dop_upstream_->close();

        main_upstream_->open();
        current_slice_num_ = num_slices_;
    }

    INLINE Optional<OutputTuple> next() {
        if (current_slice_num_ == num_slices_) {
            const auto ret = main_upstream_->next();
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

    void close() { main_upstream_->close(); }

private:
    MainUpstream *const main_upstream_;
    DopUpstream *const dop_upstream_;
    size_t num_slices_;
    OutputTuple current_tuple_{};
    size_t current_slice_num_{};
};

template <class OutputTuple, class MainUpstream, class DopUpstream>
auto makeSplitColumnDataOperator(MainUpstream *const main_upstream,
                                 DopUpstream *const dop_upstream) {
    return SplitColumnDataOperator<OutputTuple, MainUpstream, DopUpstream>(
            main_upstream, dop_upstream);
};

#endif  // CODE_GEN_OPERATORS_SPLITCOLUMNDATAOPERATOR_H
