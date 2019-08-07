#include "parquet_scan_impl.hpp"

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <type_traits>
#include <variant>
#include <vector>

#include <cassert>

#include "filesystem/filesystem.hpp"
#include "runtime/jit/memory/default_ref_counter.hpp"
#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/values/none.hpp"

namespace runtime {
namespace operators {

ParquetScanOperator* MakeParquetScanOperator(
        std::unique_ptr<FileNameOperator>&& upstream,
        std::vector<std::vector<std::shared_ptr<Predicate>>>&& range_predicates,
        std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
        const std::string& filesystem) {
    return new ParquetScanOperatorImpl(
            std::move(upstream), std::move(range_predicates),
            std::move(column_types), std::move(col_ids),
            filesystem::MakeFilesystem(filesystem));
}

auto ParquetScanOperatorImpl::type_tags()
        -> const std::map<std::string, TypeTag>& {
    static const std::map<std::string, TypeTag> map = {{"int", int32_t()},   //
                                                       {"long", int64_t()},  //
                                                       {"float", float()},   //
                                                       {"double", double()}};
    return map;
}

ParquetScanOperatorImpl::ParquetScanOperatorImpl(
        std::unique_ptr<FileNameOperator>&& upstream,
        std::vector<std::vector<std::shared_ptr<Predicate>>>&& range_predicates,
        std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
        std::unique_ptr<filesystem::FileSystem>&& fs)
    : upstream_(std::move(upstream)), fs_(std::move(fs)) {
    assert(range_predicates.size() == column_types.size());
    assert(range_predicates.size() == col_ids.size());
    for (size_t i = 0; i < col_ids.size(); i++) {
        const TypeTag type_tag = type_tags().at(column_types.at(i));
        column_infos_.push_back(ColumnInfo{col_ids.at(i),
                                           type_tag,
                                           std::move(range_predicates.at(i)),
                                           {},
                                           {}});
    }
}

void ParquetScanOperatorImpl::FetchMetaData(
        std::vector<std::string>&& file_paths) {
    for (auto const& path : file_paths) {
        auto source = fs_->OpenForRead(path);
        auto pq_file_reader = parquet::ParquetFileReader::Open(source);
        auto file_metadata = pq_file_reader->metadata();
        auto interesting_row_groups =
                ComputeInterestingRowGroups(file_metadata);

        if (interesting_row_groups.empty()) continue;

        std::unique_ptr<ParquetFileHandle> handle(new ParquetFileHandle{
                path, std::move(pq_file_reader), std::move(file_metadata),
                std::move(interesting_row_groups)});
        {
            std::lock_guard<std::mutex> lock(mutex_);
            file_handles_.push(
                    std::unique_ptr<ParquetFileHandle>(std::move(handle)));
            nfiles_to_process_--;
        }
    }
}  // namespace operators

void ParquetScanOperatorImpl::open() {
    upstream_->open();

    std::vector<std::string> file_paths;
    while (auto const tuple = upstream_->next()) {
        file_paths.push_back(tuple.value());
    }
    nfiles_to_process_ = file_paths.size();

    metadata_fetcher_ = std::thread(&ParquetScanOperatorImpl::FetchMetaData,
                                    this, std::move(file_paths));
}

std::shared_ptr<runtime::values::Value> ParquetScanOperatorImpl::next() {
    // Fetch new row group if no batches left in row group
    if (batch_index_ >= num_batches_) {
        // Fetch new file if no row groups left
        if (row_group_index_ >= interesting_row_groups_.size()) {
            // Return None if no file left
            if (AreAllFilesProcessed()) {
                return std::make_shared<runtime::values::None>();
            }

            row_group_index_ = 0;

            auto const handle = PopFileHandle();
            parquet_reader_ = std::move(handle->reader);
            file_metadata_ = std::move(handle->metadata);
            interesting_row_groups_ = std::move(handle->interesting_row_groups);

            for (auto& column_info : column_infos_) {
                auto const col_id = column_info.col_id;
                column_info.col_desc = file_metadata_->schema()->Column(col_id);
            }
        }

        // We have an active file --> set up row group

        auto const current_row_group =
                interesting_row_groups_.at(row_group_index_);
        auto const num_rows =
                file_metadata_->RowGroup(current_row_group)->num_rows();
        num_batches_ = (num_rows + batch_size_ - 1) / batch_size_;
        batch_index_ = 0;

        std::shared_ptr<parquet::RowGroupReader> row_group_reader =
                parquet_reader_->RowGroup(current_row_group);
        for (auto& column_info : column_infos_) {
            auto const col_id = column_info.col_id;
            column_info.col_reader = row_group_reader->Column(col_id);
        }

        row_group_index_++;
    }

    assert(batch_index_ < num_batches_);

    // We have an active row group --> fetch new batch

    // Fetch batch of all columns
    std::vector<std::unique_ptr<runtime::values::Value>> columns;
    int32_t prev_num_values_read = 0;
    for (size_t i = 0; i < this->column_infos_.size(); i++) {
        auto [num_values_read, batch] = ReadColumnBatch(i);
        assert(i == 0 || prev_num_values_read == num_values_read);
        prev_num_values_read = num_values_read;
        columns.push_back(std::move(batch));
    }
    batch_index_++;
    assert(prev_num_values_read == batch_size_ || batch_index_ == num_batches_);

    // Pack into runtime::Tuple
    auto ret = std::make_shared<runtime::values::Tuple>();
    ret->fields = std::move(columns);
    return std::static_pointer_cast<runtime::values::Value>(ret);
}

void ParquetScanOperatorImpl::close() {
    metadata_fetcher_.join();
    upstream_->close();
}

template <typename T>
struct ColumnTypeTraits {};

template <>
struct ColumnTypeTraits<int32_t> {
    using ParquetReader = parquet::Int32Reader;
    using ParquetStatistics = parquet::Int32Statistics;
    using RangePredicate = RangePredicate<int32_t>;
    static constexpr auto kParquetType = parquet::Type::INT32;
};

template <>
struct ColumnTypeTraits<int64_t> {
    using ParquetReader = parquet::Int64Reader;
    using ParquetStatistics = parquet::Int64Statistics;
    using RangePredicate = RangePredicate<int64_t>;
    static constexpr auto kParquetType = parquet::Type::INT64;
};

template <>
struct ColumnTypeTraits<float> {
    using ParquetReader = parquet::FloatReader;
    using ParquetStatistics = parquet::FloatStatistics;
    using RangePredicate = RangePredicate<float>;
    static constexpr auto kParquetType = parquet::Type::FLOAT;
};

template <>
struct ColumnTypeTraits<double> {
    using ParquetReader = parquet::DoubleReader;
    using ParquetStatistics = parquet::DoubleStatistics;
    using RangePredicate = RangePredicate<double>;
    static constexpr auto kParquetType = parquet::Type::DOUBLE;
};

template <class T>
std::pair<int64_t, std::unique_ptr<runtime::values::Value>> ReadColumnBatchImpl(
        parquet::ColumnReader* const reader, const int batch_size) {
    auto const typed_reader =
            dynamic_cast<typename ColumnTypeTraits<T>::ParquetReader*>(reader);
    assert(typed_reader != nullptr);

    std::unique_ptr<T[]> values(new T[batch_size]);

    int64_t num_remaining = batch_size;
    int64_t result_size = 0;
    while (num_remaining > 0 && typed_reader->HasNext()) {
        int64_t num_values_read = 0;
        const uint64_t num_non_nulls_read = typed_reader->ReadBatch(
                num_remaining, nullptr, nullptr, values.get() + result_size,
                &num_values_read);
        assert(num_non_nulls_read == num_values_read);
        num_remaining -= num_values_read;
        result_size += num_values_read;
    }

    auto ret = std::make_unique<runtime::values::Array>();
    ret->data = memory::SharedPointer<char>(
            // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
            new memory::DefaultRefCounter<T[]>(values.release()));
    ret->outer_shape = {static_cast<size_t>(result_size)};
    ret->offsets = {0};
    ret->shape = {static_cast<size_t>(result_size)};

    return {result_size, std::move(ret)};
};

std::pair<int64_t, std::unique_ptr<runtime::values::Value>>
ParquetScanOperatorImpl::ReadColumnBatch(int col) {
    auto const& column_info = column_infos_.at(col);
    auto col_reader = column_info.col_reader.get();
    auto col_desc = column_info.col_desc;

    return std::visit(
            [&](auto const tag) {
                using ColumnType = std::remove_cv_t<
                        std::remove_reference_t<decltype(tag)>>;
                using Trait = ColumnTypeTraits<ColumnType>;
                assert(col_desc->physical_type() == Trait::kParquetType);
                return ReadColumnBatchImpl<ColumnType>(col_reader, batch_size_);
            },
            column_info.type_tag);
}

auto ParquetScanOperatorImpl::PopFileHandle()
        -> std::unique_ptr<ParquetFileHandle> {
    WaitForFileHandles();

    std::lock_guard<std::mutex> lock(mutex_);
    assert(!file_handles_.empty());

    auto ret = std::move(file_handles_.front());
    file_handles_.pop();
    return ret;
}

bool ParquetScanOperatorImpl::HasPendingFileHandles() {
    std::lock_guard<std::mutex> lock(mutex_);
    return file_handles_.empty() && nfiles_to_process_ > 0;
}

bool ParquetScanOperatorImpl::AreAllFilesProcessed() {
    std::lock_guard<std::mutex> lock(mutex_);
    return file_handles_.empty() && nfiles_to_process_ == 0;
}

void ParquetScanOperatorImpl::WaitForFileHandles() {
    while (HasPendingFileHandles()) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(WAIT_DOWNLOAD_MS));
    }
}

std::vector<int> ParquetScanOperatorImpl::ComputeInterestingRowGroups(
        const std::shared_ptr<parquet::FileMetaData>& file_metadata) {
    const int num_row_groups = file_metadata->num_row_groups();

    std::vector<int> interesting_row_groups;
    for (int i = 0; i < num_row_groups; i++) {
        auto const row_group = file_metadata->RowGroup(i);

        bool has_overlap = true;
        for (const auto& column_info : column_infos_) {
            const auto col_id = column_info.col_id;
            const auto& range_predicates = column_info.range_predicates;

            if (range_predicates.empty()) continue;

            auto const statistics =
                    row_group->ColumnChunk(col_id)->statistics();

            if (!EvaluateRangePredicates(statistics, range_predicates,
                                         column_info.type_tag)) {
                has_overlap = false;
                break;
            }
        }

        if (has_overlap) {
            interesting_row_groups.push_back(i);
        }
    }
    return interesting_row_groups;
}

template <typename ColumnType>
bool EvaluateRangePredicatesImpl(
        const std::shared_ptr<parquet::RowGroupStatistics>& statistics,
        const std::vector<std::shared_ptr<Predicate>>& range_predicates) {
    using Traits = ColumnTypeTraits<ColumnType>;
    using Statistics = typename Traits::ParquetStatistics;
    using RangePredicate = typename Traits::RangePredicate;

    auto const typed_statistics = dynamic_cast<Statistics*>(statistics.get());
    assert(typed_statistics != nullptr);
    const auto min = typed_statistics->min();
    const auto max = typed_statistics->max();

    bool res = false;
    for (const auto& p : range_predicates) {
        auto const typed_predicate = dynamic_cast<RangePredicate*>(p.get());
        assert(typed_predicate != nullptr);
        res = res || (typed_predicate->lo <= max &&  //
                      typed_predicate->hi >= min);
    }
    return res;
}

bool ParquetScanOperatorImpl::EvaluateRangePredicates(
        const std::shared_ptr<parquet::RowGroupStatistics>& statistics,
        const std::vector<std::shared_ptr<Predicate>>& range_predicates,
        const TypeTag& type_tag) {
    return std::visit(
            [&](auto const tag) {
                using ColumnType = std::remove_cv_t<
                        std::remove_reference_t<decltype(tag)>>;
                using Trait = ColumnTypeTraits<ColumnType>;
                return EvaluateRangePredicatesImpl<ColumnType>(
                        statistics, range_predicates);
            },
            type_tag);
}

}  // namespace operators
}  // namespace runtime
