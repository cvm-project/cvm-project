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

#endif  // CPP_UTILS_H
