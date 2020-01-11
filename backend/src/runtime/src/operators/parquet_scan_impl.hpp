#ifndef OPERATORS_PARQUET_SCAN_IMPL_HPP
#define OPERATORS_PARQUET_SCAN_IMPL_HPP

#include <deque>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include <parquet/column_reader.h>
#include <parquet/file_reader.h>
#include <parquet/metadata.h>
#include <parquet/statistics.h>

#include "filesystem/filesystem.hpp"
#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/operators/parquet_scan.hpp"
#include "runtime/jit/values/array.hpp"
#include "runtime/jit/values/tuple.hpp"

namespace runtime {
namespace operators {

class ParquetFileOperator {
public:
    struct ParquetFileHandle {
        std::string file_path;
        std::shared_ptr<parquet::ParquetFileReader> reader;
        std::shared_ptr<parquet::FileMetaData> metadata;
    };

    ParquetFileOperator(std::unique_ptr<FileNameOperator> upstream,
                        std::unique_ptr<filesystem::FileSystem> fs)
        : upstream_(std::move(upstream)), fs_(std::move(fs)) {}
    ParquetFileOperator(const ParquetFileOperator& other) = delete;
    ParquetFileOperator(ParquetFileOperator&& other) = delete;
    auto operator=(const ParquetFileOperator& other)
            -> ParquetFileOperator& = delete;
    auto operator=(ParquetFileOperator&& other)
            -> ParquetFileOperator& = delete;

    ~ParquetFileOperator() { assert(!metadata_fetcher_.joinable()); }

    void open();
    auto next() -> std::optional<std::unique_ptr<ParquetFileHandle>>;
    void close();

private:
    static constexpr int32_t WAIT_DOWNLOAD_MS = 10;

    void FetchMetaData(const std::vector<std::string>& file_paths);

    auto HasPendingFileHandles() -> bool;
    auto AreAllFilesProcessed() -> bool;
    void WaitForFileHandles();

    std::unique_ptr<FileNameOperator> upstream_;
    std::unique_ptr<filesystem::FileSystem> fs_;

    // Meta data fetcher thread and communication with the main thread
    std::thread metadata_fetcher_;
    std::mutex mutex_;
    int32_t nfiles_to_process_ = 0;
    std::queue<std::unique_ptr<ParquetFileHandle>> file_handles_;
};

class ParquetRowGroupOperator {
public:
    using TypeTag = std::variant<int32_t, int64_t, float, double>;

    // Return FileReader along with RowGroupReader, as the latter is only valid
    // while the former is alive
    using RowGroupReader = std::shared_ptr<parquet::RowGroupReader>;
    using FileReader = std::shared_ptr<parquet::ParquetFileReader>;
    using OutputType = std::pair<RowGroupReader, FileReader>;

    ParquetRowGroupOperator(std::unique_ptr<ParquetFileOperator> upstream,
                            std::vector<std::vector<std::shared_ptr<Predicate>>>
                                    range_predicates,
                            std::vector<std::string> column_types,
                            std::vector<int> col_ids)
        : upstream_(std::move(upstream)),
          column_infos_(MakeColumnInfos(std::move(range_predicates),
                                        std::move(column_types),
                                        std::move(col_ids))) {}

    void open();
    auto next() -> std::optional<OutputType>;
    void close();

    static auto type_tags() -> const std::map<std::string, TypeTag>&;

private:
    struct ColumnInfo {
        int col_id;
        TypeTag type_tag;
        std::vector<std::shared_ptr<Predicate>> range_predicates;
    };

    static auto MakeColumnInfos(
            std::vector<std::vector<std::shared_ptr<Predicate>>>
                    range_predicates,
            std::vector<std::string> column_types, std::vector<int> col_ids)
            -> std::vector<ColumnInfo>;

    auto ComputeInterestingRowGroups(
            const std::shared_ptr<parquet::FileMetaData>& file_metadata)
            -> std::vector<int>;

    static auto EvaluateRangePredicates(
            const std::shared_ptr<parquet::RowGroupStatistics>& statistics,
            const std::vector<std::shared_ptr<Predicate>>& range_predicates,
            const TypeTag& type_tag) -> bool;

    const std::vector<ColumnInfo> column_infos_;
    const std::unique_ptr<ParquetFileOperator> upstream_;
    FileReader parquet_reader_;
    std::vector<int> interesting_row_groups_;
    int64_t row_group_index_ = 0;
};

class ParquetScanOperatorImpl : public ValueOperator {
public:
    using TypeTag = ParquetRowGroupOperator::TypeTag;

    ParquetScanOperatorImpl(std::unique_ptr<ParquetRowGroupOperator> upstream,
                            std::vector<std::string> column_types,
                            std::vector<int> col_ids)
        : upstream_(std::move(upstream)),
          column_infos_(MakeColumnInfos(std::move(column_types),
                                        std::move(col_ids))) {}

    void open() override;
    auto next() -> std::shared_ptr<runtime::values::Value> override;
    void close() override;

private:
    using ColumnReader = std::shared_ptr<parquet::ColumnReader>;
    using FileReader = std::shared_ptr<parquet::ParquetFileReader>;
    using ColumnReaders = std::vector<ColumnReader>;

    struct ColumnInfo {
        int col_id;
        TypeTag type_tag;
    };

    static auto MakeColumnInfos(std::vector<std::string> column_types,
                                std::vector<int> col_ids)
            -> std::vector<ColumnInfo>;

    auto ReadColumnBatch(int col)
            -> std::pair<int64_t, std::unique_ptr<runtime::values::Value>>;

    // "Iterator" state
    size_t num_batches_ = 0;
    size_t batch_index_ = 0;
    const std::vector<ColumnInfo> column_infos_;
    ColumnReaders col_readers_;
    FileReader file_reader_;
    const size_t max_pending_column_readers_ = 2;
    std::deque<std::tuple<int64_t, FileReader, std::future<ColumnReaders>>>
            pending_column_readers_;

    // Operator configuration
    const int64_t batch_size_ = 1UL << 15U;
    const std::unique_ptr<ParquetRowGroupOperator> upstream_;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_PARQUET_SCAN_IMPL_HPP
