#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include <memory>

#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/values/value.hpp"

#define INLINE __attribute__((always_inline))

template <class StdTuple>
auto INLINE StdTupleToTuple(const StdTuple &tuple);

template <class Tuple>
auto INLINE TupleToStdTuple(const Tuple &tuple);

template <class Tuple>
std::shared_ptr<runtime::values::Value> TupleToValue(const Optional<Tuple> &t);

template <class Tuple>
Optional<Tuple> ValueToTuple(const std::shared_ptr<runtime::values::Value> &v);

template <typename T, typename U>
auto constexpr max(T x, U y) ->
        typename std::remove_reference<decltype(x > y ? x : y)>::type {
    return x > y ? x : y;
}

template <typename StdTuple, std::size_t... Indexes>
static auto SplitTupleImpl(const StdTuple tuple,
                           std::index_sequence<Indexes...> /*indexes*/) {
    return std::make_pair(std::make_tuple(std::get<0>(tuple)),
                          std::make_tuple(std::get<Indexes + 1>(tuple)...));
}

template <typename... Types>
static auto SplitTuple(const std::tuple<Types...> tuple) {
    return SplitTupleImpl(tuple,
                          std::make_index_sequence<sizeof...(Types) - 1u>());
}

#endif  // CPP_UTILS_H
