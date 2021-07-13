#include "materialize_parquet_impl.hpp"

#include <cassert>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <arrow/array.h>
#include <arrow/buffer.h>
#include <arrow/builder.h>
#include <arrow/io/file.h>
#include <arrow/record_batch.h>
#include <arrow/table.h>
#include <arrow/type.h>
#include <nlohmann/json.hpp>
#include <parquet/arrow/writer.h>

#include "filesystem/filesystem.hpp"
#include "operators/arrow_helpers.hpp"
#include "operators/value_to_record_batch.hpp"
#include "runtime/jit/values/atomics.hpp"
#include "runtime/jit/values/none.hpp"
#include "runtime/jit/values/tuple.hpp"

namespace runtime::operators {

void from_json(const nlohmann::json& j,
               runtime::operators::ParquetProperties& properties) {
    if (j.contains("encoding")) {
        properties.encoding = j.at("encoding").get<parquet::Encoding::type>();
    }
    if (j.contains("compression")) {
        properties.compression =
                j.at("compression").get<parquet::Compression::type>();
    }
    if (j.contains("enable_dictionary")) {
        properties.enable_dictionary = j.at("enable_dictionary");
    }
    if (j.contains("enable_statistics")) {
        properties.enable_statistics = j.at("enable_statistics");
    }
}

}  // namespace runtime::operators

namespace parquet {

void from_json(const nlohmann::json& j,
               // NOLINTNEXTLINE(google-runtime-references)
               parquet::WriterProperties::Builder& properties_builder) {
    // Global properties
    if (j.contains("dictionary_pagesize_limit")) {
        properties_builder.dictionary_pagesize_limit(
                j.at("dictionary_pagesize_limit").get<int64_t>());
    }
    if (j.contains("write_batch_size")) {
        properties_builder.write_batch_size(
                j.at("write_batch_size").get<int64_t>());
    }
    if (j.contains("max_row_group_length")) {
        properties_builder.max_row_group_length(
                j.at("max_row_group_length").get<int64_t>());
    }
    if (j.contains("data_pagesize")) {
        properties_builder.data_pagesize(j.at("data_pagesize").get<int64_t>());
    }
    if (j.contains("max_statistics_size")) {
        properties_builder.max_statistics_size(
                j.at("max_statistics_size").get<size_t>());
    }

    // Global properties via ParquetProperties
    auto const properties = j.get<runtime::operators::ParquetProperties>();
    if (properties.encoding.has_value()) {
        properties_builder.encoding(properties.encoding.value());
    }
    if (properties.compression.has_value()) {
        properties_builder.compression(properties.compression.value());
    }
    if (properties.enable_dictionary.has_value()) {
        if (properties.enable_dictionary.value()) {
            properties_builder.enable_dictionary();
        } else {
            properties_builder.disable_dictionary();
        }
    }
    if (properties.enable_statistics.has_value()) {
        if (properties.enable_statistics.value()) {
            properties_builder.enable_statistics();
        } else {
            properties_builder.disable_statistics();
        }
    }

    // Per-path properties (via ParquetProperties)
    const auto jpath_properties =
            j.value("path_properties", nlohmann::json::object());
    for (auto const& it : jpath_properties.items()) {
        auto const path_properties =
                it.value().get<runtime::operators::ParquetProperties>();
        const std::string& path = it.key();
        if (path_properties.encoding.has_value()) {
            properties_builder.encoding(path, path_properties.encoding.value());
        }
        if (path_properties.compression.has_value()) {
            properties_builder.compression(path,
                                           path_properties.compression.value());
        }
        if (path_properties.enable_dictionary.has_value()) {
            if (path_properties.enable_dictionary.value()) {
                properties_builder.enable_dictionary(path);
            } else {
                properties_builder.disable_dictionary(path);
            }
        }
        if (path_properties.enable_statistics.has_value()) {
            if (path_properties.enable_statistics.value()) {
                properties_builder.enable_statistics(path);
            } else {
                properties_builder.disable_statistics(path);
            }
        }
    }
}

}  // namespace parquet

namespace runtime::operators {

auto MaterializeParquetOperatorImpl::next()
        -> std::shared_ptr<runtime::values::Value> {
    if (has_returned_) {
        return std::make_shared<values::None>();
    };

    // Read config and create writer properties
    conf_upstream_->open();
    auto const ret = conf_upstream_->next();
    assert(conf_upstream_->next()->as<runtime::values::None>() != nullptr);
    conf_upstream_->close();

    auto* const tuple_value = ret->as<runtime::values::Tuple>();
    auto* const string_value =
            tuple_value->fields[0]->as<runtime::values::String>();
    auto const conf_str = string_value->value;

    nlohmann::json jconfig = nlohmann::json::parse(conf_str);

    auto properties_builder =
            jconfig.value("writer_properties", nlohmann::json::object())
                    .get<parquet::WriterProperties::Builder>();
    properties_builder.created_by("JITQ :)");
    const auto writer_properties = properties_builder.build();

    const std::string filename = jconfig.at("filename");
    const std::string filesystem = jconfig.at("filesystem");

    // Open Parquet file writer
    auto const fs = filesystem::MakeFilesystem(filesystem);
    auto output_stream = fs->OpenForWrite(filename);

    std::unique_ptr<parquet::arrow::FileWriter> file_writer;
    operators::ThrowIfNotOK(parquet::arrow::FileWriter::Open(
            *schema_, arrow::default_memory_pool(), output_stream,
            writer_properties, &file_writer));

    // Consume upstream
    main_upstream_->open();

    std::vector<std::shared_ptr<arrow::RecordBatch>> current_row_group;
    size_t current_row_group_size = 0;
    const size_t target_num_rows_per_row_group =
            jconfig.value("target_num_rows_per_row_group", 10 * 1000 * 1000);
    while (true) {
        auto const input = main_upstream_->next();

        if (input) {
            current_row_group.push_back(input.value());
            current_row_group_size += input.value()->num_rows();
        }

        if (current_row_group_size == 0) break;

        if (current_row_group_size >= target_num_rows_per_row_group || !input) {
            auto const maybe_table =
                    arrow::Table::FromRecordBatches(current_row_group);
            operators::ThrowIfNotOK(maybe_table);
            auto const& table = maybe_table.ValueOrDie();
            const size_t num_rows = table->num_rows();

            operators::ThrowIfNotOK(file_writer->NewRowGroup(num_rows));
            for (size_t i = 0; i < table->num_columns(); i++) {
                operators::ThrowIfNotOK(file_writer->WriteColumnChunk(
                        table->column(i), 0, num_rows));
            }

            current_row_group.clear();
            current_row_group_size = 0;
        }
    }

    operators::ThrowIfNotOK(file_writer->Close());
    operators::ThrowIfNotOK(output_stream->Close());

    main_upstream_->close();

    auto file_path = std::make_shared<values::String>();
    auto slice_from = std::make_shared<values::Int64>();
    auto slice_to = std::make_shared<values::Int64>();
    auto num_slices = std::make_shared<values::Int64>();

    file_path->value = filename;
    slice_from->value = 0;
    slice_to->value = 1;
    num_slices->value = 1;

    auto tuple = std::make_shared<values::Tuple>();
    tuple->fields = {std::move(file_path), std::move(slice_from),
                     std::move(slice_to), std::move(num_slices)};

    has_returned_ = true;
    return tuple;
}

auto MakeMaterializeParquetOperator(
        std::unique_ptr<ValueOperator>&& main_upstream,
        std::unique_ptr<ValueOperator>&& conf_upstream,
        std::vector<std::string> column_types,
        std::vector<std::string> column_names)
        -> std::unique_ptr<ValueOperator> {
    auto const schema =
            MakeArrowSchema(std::move(column_types), std::move(column_names));
    return std::make_unique<MaterializeParquetOperatorImpl>(
            std::make_unique<ValueToRecordBatchOperator>(
                    std::move(main_upstream), schema),
            std::move(conf_upstream), schema);
}

}  // namespace runtime::operators
