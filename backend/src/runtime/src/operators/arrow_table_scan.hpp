#ifndef OPERATORS_ARROW_TABLE_SCAN_HPP
#define OPERATORS_ARROW_TABLE_SCAN_HPP

#include <memory>

#include "operators/arrow_table_operator.hpp"
#include "operators/record_batch_operator.hpp"

namespace arrow {
class RecordBatch;
class Table;
class TableBatchReader;
}  // namespace arrow

namespace runtime {
namespace operators {

struct ArrowTableScanOperator : public RecordBatchOperator {
    explicit ArrowTableScanOperator(
            std::unique_ptr<ArrowTableOperator> &&upstream)
        : upstream_(std::move(upstream)) {}

    void open() override { upstream_->open(); }
    auto next() -> std::shared_ptr<arrow::RecordBatch> override;
    void close() override { upstream_->close(); }

private:
    const std::unique_ptr<ArrowTableOperator> upstream_;
    std::shared_ptr<arrow::Table> current_table_;
    std::shared_ptr<arrow::TableBatchReader> current_batch_reader_;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_ARROW_TABLE_SCAN_HPP
