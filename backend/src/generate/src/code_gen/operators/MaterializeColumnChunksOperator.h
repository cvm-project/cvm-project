#ifndef CODE_GEN_CPP_OPERATORS_MATERIALIZECOLUMNCHUNKSOPERATOR_H
#define CODE_GEN_CPP_OPERATORS_MATERIALIZECOLUMNCHUNKSOPERATOR_H

#include "Utils.h"
#include "runtime/jit/memory/default_ref_counter.hpp"
#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/operators/optional.hpp"

template <class OutputTuple, class Upstream, class... InputTypes>
class MaterializeColumnChunksOperator {
    using InputTuple = decltype(std::declval<Upstream>().next().value());
    using StdInputTuple = decltype(TupleToStdTuple(std::declval<InputTuple>()));
    using StdOutputTuple =
            decltype(TupleToStdTuple(std::declval<OutputTuple>()));
    using IndexSequence = std::make_index_sequence<sizeof...(InputTypes)>;

public:
    MaterializeColumnChunksOperator(Upstream *const upstream)
        : upstream_(upstream), max_column_size_(100) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<OutputTuple> next() {
        auto columns = MakeColumns(max_column_size_);
        size_t column_size;
        for (column_size = 0; column_size < max_column_size_; column_size++) {
            const auto input = upstream_->next();
            if (!input) break;
            auto const row = TupleToStdTuple(input.value());
            AssignRow(columns, row, column_size);
        }
        if (column_size == 0) {
            return {};
        }
        SetSize(&columns, column_size);
        return StdTupleToTuple(columns);
    }

    INLINE void close() { upstream_->close(); }

private:
    template <typename ElementType>
    auto static MakeSharedPointer(const size_t num_elements) {
        using ElementTuple = decltype(
                StdTupleToTuple(std::declval<std::tuple<ElementType>>()));
        return runtime::memory::SharedPointer<ElementTuple>(
                new runtime::memory::DefaultRefCounter<ElementTuple[]>(
                        new ElementTuple[num_elements]));
    }

    template <std::size_t... I>
    auto static MakeColumnsImpl(const size_t num_rows,
                                std::index_sequence<I...> /*tag*/) {
        return std::make_tuple(std::tuple_element_t<I, StdOutputTuple>{
                MakeSharedPointer<std::tuple_element_t<I, StdInputTuple>>(
                        num_rows),
                num_rows, 0, num_rows}...);
    }

    auto static MakeColumns(const size_t num_rows) {
        return MakeColumnsImpl(num_rows, IndexSequence());
    }

    template <std::size_t... I>
    void static INLINE AssignRowImpl(const StdOutputTuple &columns,
                                     const StdInputTuple &row,
                                     const size_t index,
                                     std::index_sequence<I...>) {
        ((std::get<I>(columns).data[index].v0 = std::get<I>(row)), ...);
    }

    void static INLINE AssignRow(const StdOutputTuple &columns,
                                 const StdInputTuple &row, const size_t index) {
        return AssignRowImpl(columns, row, index, IndexSequence());
    }

    template <std::size_t... I>
    void static INLINE SetSizeImpl(StdOutputTuple *const columns,
                                   const size_t size,
                                   std::index_sequence<I...>) {
        ((std::get<I>(*columns).shape[0] = size), ...);
        ((std::get<I>(*columns).outer_shape[0] = size), ...);
    }

    void static INLINE SetSize(StdOutputTuple *const columns,
                               const size_t size) {
        return SetSizeImpl(columns, size, IndexSequence());
    }

    Upstream *const upstream_;
    const size_t max_column_size_;
};

template <class OutputTuple, class Upstream, class... InputTypes>
auto makeMaterializeColumnChunksOperator(Upstream *const upstream,
                                         std::tuple<InputTypes...> /*tag*/) {
    return MaterializeColumnChunksOperator<OutputTuple, Upstream,
                                           InputTypes...>(upstream);
};

template <class OutputTuple, class Upstream>
auto makeMaterializeColumnChunksOperator(Upstream *const upstream) {
    using InputTuple = decltype(upstream->next().value());
    using StdInputTuple = decltype(TupleToStdTuple(std::declval<InputTuple>()));
    return makeMaterializeColumnChunksOperator<OutputTuple, Upstream>(
            upstream, StdInputTuple());
};

#endif  // CODE_GEN_CPP_OPERATORS_MATERIALIZECOLUMNCHUNKSOPERATOR_H
