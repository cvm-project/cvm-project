#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#define INLINE __attribute__((always_inline))

template <class StdTuple>
auto INLINE StdTupleToTuple(const StdTuple &tuple);

template <class Tuple>
auto INLINE TupleToStdTuple(const Tuple &tuple);

#endif  // CPP_UTILS_H
