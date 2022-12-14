//
// Created by sabir on 04.07.17.
//

#ifndef CODE_GEN_OPERATORS_ROWSCANOPERATOR_H
#define CODE_GEN_OPERATORS_ROWSCANOPERATOR_H

#include <tuple>

#include "Utils.h"
#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/operators/optional.hpp"

template <class Tuple, bool kAddIndex, class Upstream>
class RowScanOperator {
    using InputTuple = decltype(std::declval<Upstream>().next().value());
    using InputDataPtr = typename std::remove_reference<decltype(
            std::declval<InputTuple>().v0.data)>::type;
    using InputInnerTuple = typename std::remove_reference<decltype(
            std::declval<InputDataPtr>()[0])>::type;

public:
    RowScanOperator(Upstream *const upstream) : upstream_(upstream) {}

    INLINE void open() {
        upstream_->open();
        current_index_ = 0;
        last_index_ = 0;
        values_ = InputDataPtr(nullptr);
    }

    INLINE Optional<Tuple> next() {
        while (current_index_ >= last_index_) {
            const auto ret = upstream_->next();
            if (!ret) return {};

            const auto input = ret.value().v0;
            current_index_ = input.offsets[0];
            last_index_ = current_index_ + input.shape[0];
            values_ = input.data;
        }

        const Tuple r = BuildResultTuple();
        current_index_++;
        return r;
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    size_t current_index_;
    size_t last_index_;
    InputDataPtr values_;

    INLINE Tuple BuildResultTuple() {
        auto const input_tuple = values_[current_index_];
        if constexpr (kAddIndex) {
            auto const std_input_tuple = TupleToStdTuple(input_tuple);
            std::tuple<long> std_index_tuple{current_index_};
            auto const ret_tuple =
                    std::tuple_cat(std_index_tuple, std_input_tuple);
            return StdTupleToTuple(ret_tuple);
        } else {
            return input_tuple;
        }
    }
};

template <class Tuple, bool kAddIndex, class Upstream>
RowScanOperator<Tuple, kAddIndex, Upstream> makeRowScanOperator(
        Upstream *const upstream) {
    return RowScanOperator<Tuple, kAddIndex, Upstream>(upstream);
}

#endif  // CODE_GEN_OPERATORS_ROWSCANOPERATOR_H
