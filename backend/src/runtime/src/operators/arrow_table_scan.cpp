#include "arrow_table_scan.hpp"

#include <memory>

#include <arrow/record_batch.h>
#include <arrow/table.h>

#include "arrow_helpers.hpp"

namespace runtime::operators {

auto ArrowTableScanOperator::next() -> std::shared_ptr<arrow::RecordBatch> {
    while (true) {
        if (!current_table_) {
            current_table_ = upstream_->next();
            if (!current_table_) {
                // End-of-stream
                break;
            }

            current_batch_reader_.reset(
                    new arrow::TableBatchReader(*current_table_));
        }

        std::shared_ptr<arrow::RecordBatch> record_batch;
        operators::ThrowIfNotOK(current_batch_reader_->ReadNext(&record_batch));

        if (record_batch) {
            // Found record batch, return
            return record_batch;
        }

        // Current table doesn't have record batches left; reset such that next
        // table is fetched from upstream
        current_batch_reader_.reset();
        current_table_.reset();
    }
    return {};
}

}  // namespace runtime::operators
