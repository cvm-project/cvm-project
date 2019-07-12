#ifndef RUNTIME_OPERATORS_PARQUET_SCAN_IMPL_HPP
#define RUNTIME_OPERATORS_PARQUET_SCAN_IMPL_HPP

#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include <parquet/column_reader.h>
#include <parquet/file_reader.h>
#include <parquet/metadata.h>
#include <parquet/statistics.h>

#include "runtime/filesystem/filesystem.hpp"
#include "runtime/operators/optional.hpp"
#include "runtime/operators/parquet_scan.hpp"
#include "runtime/values/array.hpp"
#include "runtime/values/tuple.hpp"

namespace runtime {
namespace operators {

class ParquetScanOperatorImpl : public ParquetScanOperator {
public:
    using TypeTag = std::variant<int32_t, int64_t, float, double>;

    ParquetScanOperatorImpl(
            std::unique_ptr<FileNameOperator>&& upstream,
            std::vector<std::vector<std::shared_ptr<Predicate>>>&&
                    range_predicates,
            std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
            std::unique_ptr<filesystem::FileSystem>&& fs);

    static const std::map<std::string, ParquetScanOperatorImpl::TypeTag>&
    type_tags();

    void open() override;
    std::shared_ptr<runtime::values::Value> next() override;
    void close() override;

private:
    struct ParquetFileHandle {
        std::string file_path;
        std::unique_ptr<parquet::ParquetFileReader> reader;
        std::shared_ptr<parquet::FileMetaData> metadata;
        std::vector<int> interesting_row_groups;
    };

    struct ColumnInfo {
        int col_id;
        TypeTag type_tag;
        std::vector<std::shared_ptr<Predicate>> range_predicates;

        const parquet::ColumnDescriptor* col_desc;
        std::shared_ptr<parquet::ColumnReader> col_reader;
    };

    static constexpr int32_t WAIT_DOWNLOAD_MS = 10;

    void FetchMetaData(std::vector<std::string>&& file_paths);

    std::pair<int64_t, std::unique_ptr<runtime::values::Value>> ReadColumnBatch(
            int col);

    std::unique_ptr<ParquetFileHandle> PopFileHandle();
    bool HasPendingFileHandles();
    bool AreAllFilesProcessed();
    void WaitForFileHandles();

    std::vector<int> ComputeInterestingRowGroups(
            const std::shared_ptr<parquet::FileMetaData>& file_metadata);

    bool EvaluateRangePredicates(
            const std::shared_ptr<parquet::RowGroupStatistics>& statistics,
            const std::vector<std::shared_ptr<Predicate>>& range_predicates,
            const TypeTag& type_tag);

    // Meta data fetcher thread and communication with the main thread
    std::thread metadata_fetcher_;
    std::mutex mutex_;
    int32_t nfiles_to_process_ = 0;
    std::queue<std::unique_ptr<ParquetFileHandle>> file_handles_;

    // "Iterator" state
    size_t num_batches_ = 0;
    size_t batch_index_ = 0;
    int64_t row_group_index_ = 0;
    std::vector<int> interesting_row_groups_;
    std::unique_ptr<parquet::ParquetFileReader> parquet_reader_;
    std::shared_ptr<parquet::FileMetaData> file_metadata_;
    std::vector<ColumnInfo> column_infos_;

    // Operator configuration
    const int64_t batch_size_ = 100;
    std::unique_ptr<FileNameOperator> upstream_;
    std::unique_ptr<filesystem::FileSystem> fs_;
};

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_OPERATORS_PARQUET_SCAN_IMPL_HPP
