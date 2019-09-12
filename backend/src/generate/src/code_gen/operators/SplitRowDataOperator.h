#ifndef CODE_GEN_OPERATORS_SPLITROWDATAOPERATOR_H
#define CODE_GEN_OPERATORS_SPLITROWDATAOPERATOR_H

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class OutputTuple, class MainUpstream, class DopUpstream>
class SplitRowDataOperator {
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

public:
    SplitRowDataOperator(MainUpstream *const main_upstream,
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

        const auto data = current_tuple_.v0.data;
        const auto outer_shape = current_tuple_.v0.outer_shape[0];
        const auto offset = current_tuple_.v0.offsets[0];
        const auto size = current_tuple_.v0.shape[0];

        const auto first_idx =
                ComputeFirstIndex(size, current_slice_num_, num_slices_);
        const auto last_idx =
                ComputeLastIndex(size, current_slice_num_, num_slices_);

        current_slice_num_++;

        return OutputTuple{data, outer_shape, offset + first_idx,
                           last_idx - first_idx};
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
auto makeSplitRowDataOperator(MainUpstream *const main_upstream,
                              DopUpstream *const dop_upstream) {
    return SplitRowDataOperator<OutputTuple, MainUpstream, DopUpstream>(
            main_upstream, dop_upstream);
};

#endif  // CODE_GEN_OPERATORS_SPLITROWDATAOPERATOR_H
