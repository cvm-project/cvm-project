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
std::shared_ptr<Predicate> MakeRangePredicate(const T& lo, const T& hi) {
    // cppcheck-suppress internalAstError
    return std::shared_ptr<Predicate>(new RangePredicate<T>(lo, hi));
}

struct FileNameOperator {
    virtual ~FileNameOperator() = default;
    virtual void open() = 0;
    virtual Optional<std::string> next() = 0;
    virtual void close() = 0;
};

ValueOperator* MakeParquetScanOperator(
        std::unique_ptr<FileNameOperator>&& upstream,
        std::vector<std::vector<std::shared_ptr<Predicate>>>&& range_predicates,
        std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
        const std::string& filesystem);

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_PARQUET_SCAN_HPP
