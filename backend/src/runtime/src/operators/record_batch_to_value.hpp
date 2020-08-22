#ifndef OPERATORS_RECORD_BATCH_TO_VALUE_HPP
#define OPERATORS_RECORD_BATCH_TO_VALUE_HPP

#include <any>
#include <memory>

#include "operators/record_batch_operator.hpp"
#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/operators/value_operator.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime::operators {

struct AnyRefCounter : public memory::RefCounter {
    AnyRefCounter(void* const pointer, std::any handle)
        : RefCounter(pointer), handle_(std::move(handle)) {}

    AnyRefCounter(const AnyRefCounter& other) = delete;
    AnyRefCounter(AnyRefCounter&& other) = delete;
    auto operator=(const AnyRefCounter& other) -> AnyRefCounter& = delete;
    auto operator=(AnyRefCounter&& other) -> AnyRefCounter& = delete;
    ~AnyRefCounter() override = default;

private:
    std::any handle_;
};

auto ConvertRecordBatchToValue(
        const std::shared_ptr<arrow::RecordBatch>& record_batch)
        -> std::shared_ptr<values::Value>;

struct RecordBatchToValueOperator : public ValueOperator {
    explicit RecordBatchToValueOperator(
            std::unique_ptr<RecordBatchOperator>&& upstream)
        : upstream_(std::move(upstream)) {}

    void open() override { upstream_->open(); }
    auto next() -> std::shared_ptr<values::Value> override;
    void close() override { upstream_->close(); }

private:
    const std::unique_ptr<RecordBatchOperator> upstream_;
};

}  // namespace runtime::operators

#endif  // OPERATORS_RECORD_BATCH_TO_VALUE_HPP
