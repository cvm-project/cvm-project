#ifndef RUNTIME_OPERATORS_MATERIALIZE_PARQUET_HPP
#define RUNTIME_OPERATORS_MATERIALIZE_PARQUET_HPP

#include <memory>
#include <string>
#include <vector>

#include "runtime/operators/optional.hpp"
#include "runtime/operators/value_operator.hpp"
#include "runtime/values/value.hpp"

namespace runtime {
namespace operators {

struct MaterializeParquetOperator {
    virtual ~MaterializeParquetOperator() = default;
    virtual void open() = 0;
    virtual Optional<std::string> next() = 0;
    virtual void close() = 0;
};

std::unique_ptr<MaterializeParquetOperator> MakeMaterializeParquetOperator(
        std::unique_ptr<ValueOperator> main_upstream,
        std::unique_ptr<ValueOperator> conf_upstream,
        std::vector<std::string> column_types,
        std::vector<std::string> column_names);

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_OPERATORS_MATERIALIZE_PARQUET_HPP
