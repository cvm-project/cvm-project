#include "record_batch_to_value.hpp"

#include <memory>
#include <set>
#include <vector>

#include <arrow/array/data.h>
#include <arrow/record_batch.h>
#include <arrow/table.h>
#include <arrow/type.h>

#include "arrow_helpers.hpp"
#include "runtime/jit/values/array.hpp"
#include "runtime/jit/values/none.hpp"
#include "runtime/jit/values/tuple.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime::operators {

auto ConvertRecordBatchToValue(
        const std::shared_ptr<arrow::RecordBatch> &record_batch)
        -> std::shared_ptr<values::Value> {
    auto const schema = record_batch->schema();
    auto const num_columns = record_batch->num_columns();
    auto const num_rows = record_batch->num_rows();

    std::vector<std::shared_ptr<values::Value>> columns;
    for (size_t i = 0; i < num_columns; i++) {
        auto const column_data = record_batch->column_data(i);

        // Only support single, dense allocation
        assert(column_data->length == num_rows);
        assert(column_data->null_count == 0);
        assert(column_data->offset == 0);
        assert(column_data->buffers.size() == 2);  // XXX Reading from Parquet
        assert(!column_data->buffers.at(0));       // seems to produce an empty
        assert(column_data->buffers.at(1));        // buffer point at .at(0)
        assert(column_data->child_data.empty());
        assert(!column_data->dictionary);

        // Assert that data type is supported
        auto const type = column_data->type;
        static const std::set<arrow::Type::type> kSupportedTypes{
                arrow::Type::type::INT32,   //
                arrow::Type::type::INT64,   //
                arrow::Type::type::FLOAT,   //
                arrow::Type::type::DOUBLE,  //
        };
        assert(kSupportedTypes.count(type->id()) > 0);

        // Get arrow buffer and underlying pointer
        auto buffer = column_data->buffers.at(1);
        assert(buffer);

        // XXX: Is this const_cast safe?
        auto *const ptr = const_cast<uint8_t *>(buffer->data());

        // Create array value
        auto column = std::make_unique<runtime::values::Array>();
        column->data = memory::SharedPointer<char>(
                new AnyRefCounter(ptr, std::move(buffer)));
        column->outer_shape = {static_cast<size_t>(num_rows)};
        column->offsets = {0};
        column->shape = {static_cast<size_t>(num_rows)};

        columns.emplace_back(std::move(column));
    }

    // Pack into values::Tuple
    auto ret = std::make_shared<values::Tuple>();
    ret->fields = std::move(columns);
    return std::static_pointer_cast<values::Value>(ret);
}

auto RecordBatchToValueOperator::next() -> std::shared_ptr<values::Value> {
    auto const input = upstream_->next();
    if (!input) {
        return std::make_shared<values::None>();
    }
    return ConvertRecordBatchToValue(input);
}

}  // namespace runtime::operators
