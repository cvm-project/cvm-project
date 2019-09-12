#ifndef CODE_GEN_OPERATORS_SPLITRANGEOPERATOR_H
#define CODE_GEN_OPERATORS_SPLITRANGEOPERATOR_H

#include <omp.h>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class OutputTuple, class MainUpstream, class DopUpstream>
class SplitRangeOperator {
public:
    typedef decltype(OutputTuple().v0) ValueType;

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
    SplitRangeOperator(MainUpstream *const main_upstream,
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

        const auto from = current_tuple_.v0;
        const auto to = current_tuple_.v1;
        const auto step = current_tuple_.v2;

        const auto ret =
                OutputTuple{ComputeStartIndex(from, to, step,
                                              current_slice_num_, num_slices_),
                            ComputeEndIndex(from, to, step, current_slice_num_,
                                            num_slices_),
                            step};

        current_slice_num_++;
        return ret;
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
auto makeSplitRangeOperator(MainUpstream *const main_upstream,
                            DopUpstream *const dop_upstream) {
    return SplitRangeOperator<OutputTuple, MainUpstream, DopUpstream>(
            main_upstream, dop_upstream);
};

#endif  // CODE_GEN_OPERATORS_SPLITRANGEOPERATOR_H
