#ifndef RUNTIME_JIT_OPERATORS_MATERIALIZE_PARQUET_HPP
#define RUNTIME_JIT_OPERATORS_MATERIALIZE_PARQUET_HPP

#include <memory>
#include <string>
#include <vector>

#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/operators/value_operator.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace operators {

struct MaterializeParquetOperator {
    virtual ~MaterializeParquetOperator() = default;
    virtual void open() = 0;
    virtual auto next() -> Optional<std::string> = 0;
    virtual void close() = 0;
};

auto MakeMaterializeParquetOperator(
        std::unique_ptr<ValueOperator> main_upstream,
        std::unique_ptr<ValueOperator> conf_upstream,
        std::vector<std::string> column_types,
        std::vector<std::string> column_names)
        -> std::unique_ptr<MaterializeParquetOperator>;

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_MATERIALIZE_PARQUET_HPP
