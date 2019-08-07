#ifndef CPP_COLUMNSCANOPERATOR_H
#define CPP_COLUMNSCANOPERATOR_H

#include <cstring>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Utils.h"
#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/operators/optional.hpp"

template <class Tuple, bool kAddIndex, class Upstream, class... InputTypes>
class ColumnScanOperator {
public:
    using Columns = std::tuple<runtime::memory::SharedPointer<InputTypes>...>;
    using UnindexedOutputStdTuple =
            std::tuple<decltype(std::declval<InputTypes>().v0)...>;
    using IndexSequence =
            std::make_index_sequence<std::tuple_size<Columns>::value>;

private:
    void ResetColumns() { columns_ = decltype(columns_)(); }

    template <class InputStdTuple, std::size_t... I>
    auto static MakeColumnsImpl(const InputStdTuple &input_tuple,
                                std::index_sequence<I...>) {
        assert(((std::get<I>(input_tuple).shape[0] ==
                 std::get<0>(input_tuple).shape[0]) &&
                ...));
        return std::make_tuple(std::get<I>(input_tuple).data...);
    }

    template <class InputStdTuple>
    auto static MakeColumns(const InputStdTuple &input_tuple) {
        return MakeColumnsImpl(input_tuple, IndexSequence());
    }

    template <std::size_t... I>
    auto INLINE LookupImpl(const size_t index, std::index_sequence<I...>) {
        return std::make_tuple(std::get<I>(columns_)[index].v0...);
    }

    auto INLINE Lookup(const size_t index) {
        return LookupImpl(index, IndexSequence());
    }

    struct NoAddIndexFunctor {
        auto operator()(const UnindexedOutputStdTuple &tuple,
                        const size_t index) {
            return tuple;
        }
    };

    struct AddIndexFunctor {
        auto operator()(const UnindexedOutputStdTuple &tuple,
                        const long index) {
            return std::tuple_cat(std::make_tuple(index), tuple);
        }
    };

    using MakeResulTuple = typename std::conditional<kAddIndex, AddIndexFunctor,
                                                     NoAddIndexFunctor>::type;

public:
    ColumnScanOperator(Upstream *const upstream) : upstream_(upstream) {}

    INLINE void open() {
        upstream_->open();
        current_index_ = 0;
        last_index_ = 0;
        ResetColumns();
    }

    INLINE Optional<Tuple> next() {
        while (current_index_ >= last_index_) {
            const auto ret = upstream_->next();
            if (!ret) return {};

            auto const input_tuple = TupleToStdTuple(ret.value());

            const auto num_elements = std::get<0>(input_tuple).shape[0];
            current_index_ = std::get<0>(input_tuple).offsets[0];
            last_index_ = current_index_ + num_elements;
            columns_ = MakeColumns(input_tuple);
        }

        const auto output_tuple = Lookup(current_index_);
        const auto ret =
                StdTupleToTuple(MakeResulTuple()(output_tuple, current_index_));
        current_index_++;
        return ret;
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    size_t current_index_;
    size_t last_index_;
    Columns columns_;
};

template <class Tuple, bool kAddIndex, class Upstream, class... InputTypes>
auto makeColumnScanOperator(Upstream *const upstream,
                            std::tuple<InputTypes...>) {
    return ColumnScanOperator<
            Tuple, kAddIndex, Upstream,
            typename std::remove_reference<decltype(
                    std::declval<InputTypes>().data[0])>::type...>(upstream);
}

template <class Tuple, bool kAddIndex, class Upstream>
auto makeColumnScanOperator(Upstream *const upstream) {
    using InputTuple = decltype(upstream->next().value());
    using StdInputTuple = decltype(TupleToStdTuple(std::declval<InputTuple>()));
    return makeColumnScanOperator<Tuple, kAddIndex>(upstream, StdInputTuple());
}

#endif  // CPP_COLUMNSCANOPERATOR_H
