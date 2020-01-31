#include "value_to_record_batch.hpp"

#include <memory>
#include <vector>

#include <arrow/array.h>
#include <arrow/buffer.h>
#include <arrow/record_batch.h>
#include <arrow/type.h>

#include "runtime/jit/values/array.hpp"
#include "runtime/jit/values/none.hpp"
#include "runtime/jit/values/tuple.hpp"

using runtime::values::Array;
using runtime::values::None;
using runtime::values::Tuple;

namespace runtime::operators {

auto ConvertValueToRecordBatch(
        const std::shared_ptr<const values::Value> &value,
        const std::shared_ptr<arrow::Schema> &schema)
        -> std::shared_ptr<arrow::RecordBatch> {
    auto const &value_columns = value->as<Tuple>()->fields;
    assert(!value_columns.empty());
    const size_t num_rows = value_columns.at(0)->as<Array>()->shape.at(0);

    std::vector<std::shared_ptr<arrow::Array>> arrow_columns;
    arrow_columns.reserve(value_columns.size());

    for (size_t i = 0; i < value_columns.size(); i++) {
        assert(!schema->field(i)->nullable());
        const auto type = schema->field(i)->type();
        const auto fixed_width_type =
                dynamic_cast<arrow::FixedWidthType *>(type.get());
        assert(fixed_width_type != nullptr);
        const size_t type_width = fixed_width_type->bit_width() / 8;

        const auto value = value_columns.at(i)->as<Array>();
        const auto ptr = reinterpret_cast<const uint8_t *>(value->data.get());

        std::shared_ptr<arrow::Buffer> buffer(
                new arrow::Buffer(ptr, num_rows * type_width),
                BufferDeleter{value->data});
        const auto arrow_column =
                std::make_shared<arrow::PrimitiveArray>(type, num_rows, buffer);
        arrow_columns.push_back(arrow_column);
    }

    return arrow::RecordBatch::Make(schema, num_rows, arrow_columns);
}

auto ValueToRecordBatchOperator::next()
        -> Optional<std::shared_ptr<arrow::RecordBatch>> {
    auto const input = upstream_->next();
    if (dynamic_cast<None *>(input.get()) != nullptr) {
        return {};
    }
    return ConvertValueToRecordBatch(input, schema_);
}

}  // namespace runtime::operators
