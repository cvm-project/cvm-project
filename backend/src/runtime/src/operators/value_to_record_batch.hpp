#ifndef OPERATORS_VALUE_TO_RECORD_BATCH_HPP
#define OPERATORS_VALUE_TO_RECORD_BATCH_HPP

#include <memory>

#include <arrow/buffer.h>
#include <arrow/record_batch.h>
#include <arrow/type.h>

#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/operators/value_operator.hpp"

namespace runtime {
namespace operators {

struct BufferDeleter {
    runtime::memory::SharedPointer<char> value;
    void operator()(arrow::Buffer* const ptr) {
        std::default_delete<arrow::Buffer>()(ptr);
    }
};

struct ValueToRecordBatchOperator {
    ValueToRecordBatchOperator(std::unique_ptr<ValueOperator> upstream,
                               std::shared_ptr<arrow::Schema> schema)
        : upstream_(std::move(upstream)), schema_(std::move(schema)) {}

    void open() { upstream_->open(); }
    Optional<std::shared_ptr<arrow::RecordBatch>> next();
    void close() { upstream_->close(); }

private:
    const std::unique_ptr<ValueOperator> upstream_;
    const std::shared_ptr<arrow::Schema> schema_;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_VALUE_TO_RECORD_BATCH_HPP
