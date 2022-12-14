#ifndef OPERATORS_MATERIALIZE_PARQUET_IMPL_HPP
#define OPERATORS_MATERIALIZE_PARQUET_IMPL_HPP

#include <memory>
#include <optional>
#include <string>

#include <arrow/type.h>
#include <nlohmann/json.hpp>
#include <parquet/arrow/writer.h>

#include "operators/value_to_record_batch.hpp"
#include "runtime/jit/operators/materialize_parquet.hpp"
#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/operators/value_operator.hpp"

namespace runtime {
namespace operators {

struct ParquetProperties {
    std::optional<bool> enable_dictionary;
    std::optional<bool> enable_statistics;
    std::optional<parquet::Encoding::type> encoding;
    std::optional<parquet::Compression::type> compression;
};

void from_json(const nlohmann::json& j,
               // NOLINTNEXTLINE(google-runtime-references)
               runtime::operators::ParquetProperties& properties);

}  // namespace operators
}  // namespace runtime

namespace arrow {

void from_json(const nlohmann::json& j,
               // NOLINTNEXTLINE(google-runtime-references)
               parquet::WriterProperties::Builder& properties_builder);

// NOLINTNEXTLINE(google-runtime-references,modernize-avoid-c-arrays)
NLOHMANN_JSON_SERIALIZE_ENUM(  //
        Compression::type,     //
        {
                {Compression::UNCOMPRESSED, "UNCOMPRESSED"},
                {Compression::SNAPPY, "SNAPPY"},
                {Compression::GZIP, "GZIP"},
                {Compression::LZO, "LZO"},
                {Compression::BROTLI, "BROTLI"},
                {Compression::LZ4, "LZ4"},
                {Compression::ZSTD, "ZSTD"},
        })

}  // namespace arrow

namespace parquet {

// NOLINTNEXTLINE(google-runtime-references,modernize-avoid-c-arrays)
NLOHMANN_JSON_SERIALIZE_ENUM(  //
        Encoding::type,        //
        {
                {Encoding::PLAIN, "PLAIN"},
                {Encoding::PLAIN_DICTIONARY, "PLAIN_DICTIONARY"},
                {Encoding::RLE, "RLE"},
                {Encoding::BIT_PACKED, "BIT_PACKED"},
                {Encoding::DELTA_BINARY_PACKED, "DELTA_BINARY_PACKED"},
                {Encoding::DELTA_LENGTH_BYTE_ARRAY, "DELTA_LENGTH_BYTE_ARRAY"},
                {Encoding::DELTA_BYTE_ARRAY, "DELTA_BYTE_ARRAY"},
                {Encoding::RLE_DICTIONARY, "RLE_DICTIONARY"},
        })

}  // namespace parquet

namespace runtime {
namespace operators {

struct MaterializeParquetOperatorImpl : public ValueOperator {
    MaterializeParquetOperatorImpl(
            std::unique_ptr<ValueToRecordBatchOperator> main_upstream,
            std::unique_ptr<ValueOperator> conf_upstream,
            std::shared_ptr<arrow::Schema> schema)
        : main_upstream_(std::move(main_upstream)),
          conf_upstream_(std::move(conf_upstream)),
          schema_(std::move(schema)),
          has_returned_(false) {}

    void open() override {}
    auto next() -> std::shared_ptr<runtime::values::Value> override;
    void close() override {}

private:
    const std::unique_ptr<ValueToRecordBatchOperator> main_upstream_;
    const std::unique_ptr<ValueOperator> conf_upstream_;
    const std::shared_ptr<arrow::Schema> schema_;
    bool has_returned_;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_MATERIALIZE_PARQUET_IMPL_HPP
