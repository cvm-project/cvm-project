#include "parquet_scan_impl.hpp"

#include <cassert>

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

#include "filesystem/filesystem.hpp"
#include "runtime/jit/memory/default_ref_counter.hpp"
#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/values/none.hpp"

namespace runtime::operators {

auto MakeParquetScanOperator(
        std::unique_ptr<FileNameOperator> upstream,
        std::vector<std::vector<std::shared_ptr<Predicate>>> range_predicates,
        const std::vector<std::string>& column_types,
        const std::vector<int>& col_ids, const std::string& filesystem)
        -> ValueOperator* {
    auto parquet_file_op = std::make_unique<ParquetFileOperator>(
            std::move(upstream), filesystem::MakeFilesystem(filesystem));

    auto row_group_op = std::make_unique<ParquetRowGroupOperator>(
            std::move(parquet_file_op), std::move(range_predicates),
            column_types, col_ids);

    return new ParquetScanOperatorImpl(std::move(row_group_op), column_types,
                                       col_ids);
}

void ParquetFileOperator::open() {
    upstream_->open();

    std::vector<std::string> file_paths;
    while (auto const tuple = upstream_->next()) {
        file_paths.push_back(tuple.value());
    }
    nfiles_to_process_ = file_paths.size();

    metadata_fetcher_ = std::thread(&ParquetFileOperator::FetchMetaData, this,
                                    std::move(file_paths));
}

auto ParquetFileOperator::next()
        -> std::optional<std::unique_ptr<ParquetFileHandle>> {
    if (AreAllFilesProcessed()) {
        return {};
    }

    WaitForFileHandles();

    std::lock_guard<std::mutex> lock(mutex_);
    assert(!file_handles_.empty());

    auto ret = std::move(file_handles_.front());
    file_handles_.pop();
    return ret;
}

void ParquetFileOperator::close() {
    metadata_fetcher_.join();
    upstream_->close();
}

void ParquetFileOperator::FetchMetaData(
        const std::vector<std::string>& file_paths) {
    for (auto const& path : file_paths) {
        auto source = fs_->OpenForRead(path);
        auto pq_file_reader = parquet::ParquetFileReader::Open(source);
        auto file_metadata = pq_file_reader->metadata();

        std::unique_ptr<ParquetFileHandle> handle(new ParquetFileHandle{
                path, std::move(pq_file_reader), std::move(file_metadata)});
        {
            std::lock_guard<std::mutex> lock(mutex_);
            file_handles_.push(
                    std::unique_ptr<ParquetFileHandle>(std::move(handle)));
            nfiles_to_process_--;
        }
    }
}

auto ParquetFileOperator::HasPendingFileHandles() -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    return file_handles_.empty() && nfiles_to_process_ > 0;
}

auto ParquetFileOperator::AreAllFilesProcessed() -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    return file_handles_.empty() && nfiles_to_process_ == 0;
}

void ParquetFileOperator::WaitForFileHandles() {
    while (HasPendingFileHandles()) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(WAIT_DOWNLOAD_MS));
    }
}

void ParquetRowGroupOperator::open() { upstream_->open(); }

auto ParquetRowGroupOperator::next() -> std::optional<OutputType> {
    // Fetch new file if no row groups left
    while (row_group_index_ >= interesting_row_groups_.size()) {
        auto res = upstream_->next();
        if (!res.has_value()) {
            return {};
        }

        auto const& handle = *res;
        row_group_index_ = 0;

        auto const file_metadata = std::move(handle->metadata);
        parquet_reader_ = std::move(handle->reader);

        interesting_row_groups_ = ComputeInterestingRowGroups(file_metadata);
    }

    auto const current_row_group =
            interesting_row_groups_.at(row_group_index_++);
    return OutputType{parquet_reader_->RowGroup(current_row_group),
                      parquet_reader_};
}

void ParquetRowGroupOperator::close() { upstream_->close(); }

auto ParquetRowGroupOperator::type_tags()
        -> const std::map<std::string, TypeTag>& {
    static const std::map<std::string, TypeTag> map = {{"int", int32_t()},   //
                                                       {"long", int64_t()},  //
                                                       {"float", float()},   //
                                                       {"double", double()}};
    return map;
}

auto ParquetRowGroupOperator::MakeColumnInfos(
        std::vector<std::vector<std::shared_ptr<Predicate>>> range_predicates,
        std::vector<std::string> column_types, std::vector<int> col_ids)
        -> std::vector<ColumnInfo> {
    assert(range_predicates.size() == column_types.size());
    assert(range_predicates.size() == col_ids.size());
    std::vector<ColumnInfo> column_infos;
    for (size_t i = 0; i < col_ids.size(); i++) {
        const TypeTag type_tag = type_tags().at(column_types.at(i));
        column_infos.push_back(ColumnInfo{col_ids.at(i), type_tag,
                                          std::move(range_predicates.at(i))});
    }
    return column_infos;
}

auto ParquetRowGroupOperator::ComputeInterestingRowGroups(
        const std::shared_ptr<parquet::FileMetaData>& file_metadata)
        -> std::vector<int> {
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

template <typename ColumnType>
auto EvaluateRangePredicatesImpl(
        const std::shared_ptr<parquet::RowGroupStatistics>& statistics,
        const std::vector<std::shared_ptr<Predicate>>& range_predicates)
        -> bool {
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

auto ParquetRowGroupOperator::EvaluateRangePredicates(
        const std::shared_ptr<parquet::RowGroupStatistics>& statistics,
        const std::vector<std::shared_ptr<Predicate>>& range_predicates,
        const TypeTag& type_tag) -> bool {
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

void ParquetScanOperatorImpl::open() { upstream_->open(); }

auto ParquetScanOperatorImpl::next()
        -> std::shared_ptr<runtime::values::Value> {
    // Start reading next row group if no batches left in current one
    if (batch_index_ >= num_batches_) {
        while (auto res = upstream_->next()) {
            auto const [row_group_reader, file_reader] = res.value();
            auto const num_rows = row_group_reader->metadata()->num_rows();

            // Skip over row group if it is empty
            if (num_rows == 0) {
                continue;
            }

            // Start reading row group data into main-memory
            pending_column_readers_.emplace_front(
                    num_rows, file_reader,
                    std::async(
                            std::launch::async,
                            [this](auto const row_group_reader,
                                   auto const file_reader) {
                                ColumnReaders col_readers;
                                (void)file_reader;  // Needs to be alive
                                for (const auto& column_info : column_infos_) {
                                    auto const col_id = column_info.col_id;
                                    col_readers.push_back(
                                            row_group_reader->Column(col_id));
                                }
                                return col_readers;
                            },
                            row_group_reader, file_reader));

            if (pending_column_readers_.size() >= max_pending_column_readers_) {
                break;
            }
        }

        // Signal end of stream if no pending row group left
        if (pending_column_readers_.empty()) {
            return std::make_shared<runtime::values::None>();
        }

        // Return new row group as soon as it is available
        auto [num_rows, file_reader, col_readers] =
                std::move(pending_column_readers_.back());
        col_readers_ = col_readers.get();
        file_reader_ = std::move(file_reader);
        pending_column_readers_.pop_back();

        num_batches_ = (num_rows + batch_size_ - 1) / batch_size_;
        batch_index_ = 0;
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

void ParquetScanOperatorImpl::close() { upstream_->close(); }

auto ParquetScanOperatorImpl::MakeColumnInfos(
        std::vector<std::string> column_types, std::vector<int> col_ids)
        -> std::vector<ColumnInfo> {
    assert(col_ids.size() == column_types.size());
    std::vector<ColumnInfo> column_infos;
    for (size_t i = 0; i < col_ids.size(); i++) {
        const TypeTag type_tag =
                ParquetRowGroupOperator::type_tags().at(column_types.at(i));
        column_infos.push_back(ColumnInfo{col_ids.at(i), type_tag});
    }
    return column_infos;
}

template <class T>
auto ReadColumnBatchImpl(const std::shared_ptr<parquet::ColumnReader>& reader,
                         const int batch_size)
        -> std::pair<int64_t, std::unique_ptr<runtime::values::Value>> {
    auto const typed_reader =
            dynamic_cast<typename ColumnTypeTraits<T>::ParquetReader*>(
                    reader.get());
    assert(typed_reader != nullptr);

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
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
            // NOLINTNEXTLINE(modernize-avoid-c-arrays)
            new memory::DefaultRefCounter<T[]>(values.release()));
    ret->outer_shape = {static_cast<size_t>(result_size)};
    ret->offsets = {0};
    ret->shape = {static_cast<size_t>(result_size)};

    return {result_size, std::move(ret)};
};

auto ParquetScanOperatorImpl::ReadColumnBatch(int col)
        -> std::pair<int64_t, std::unique_ptr<runtime::values::Value>> {
    auto const& column_info = column_infos_.at(col);
    auto const& col_reader = col_readers_.at(col);

    return std::visit(
            [&](auto const tag) {
                using ColumnType = std::remove_cv_t<
                        std::remove_reference_t<decltype(tag)>>;
                using Trait = ColumnTypeTraits<ColumnType>;
                assert(col_reader->descr()->physical_type() ==
                       Trait::kParquetType);
                return ReadColumnBatchImpl<ColumnType>(col_reader, batch_size_);
            },
            column_info.type_tag);
}

}  // namespace runtime::operators
