#ifndef RUNTIME_JIT_OPERATORS_PARQUET_SCAN_HPP
#define RUNTIME_JIT_OPERATORS_PARQUET_SCAN_HPP

#include <memory>
#include <string>
#include <vector>

#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/operators/value_operator.hpp"

namespace runtime {
namespace operators {

class Predicate {
public:
    virtual ~Predicate() = default;
};

template <typename T>
struct RangePredicate : public Predicate {
    RangePredicate(const T& _lo, const T& _hi) : lo(_lo), hi(_hi) {}
    T lo;
    T hi;
};

template <typename T>
auto MakeRangePredicate(const T& lo, const T& hi)
        -> std::shared_ptr<Predicate> {
    // cppcheck-suppress internalAstError
    return std::shared_ptr<Predicate>(new RangePredicate<T>(lo, hi));
}

auto MakeParquetScanOperator(
        std::unique_ptr<ValueOperator> upstream,
        std::vector<std::vector<std::shared_ptr<Predicate>>> range_predicates,
        const std::vector<std::string>& column_types,
        const std::vector<int>& col_ids, const std::string& filesystem)
        -> ValueOperator*;

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_PARQUET_SCAN_HPP
