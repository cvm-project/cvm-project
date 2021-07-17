#include "exchange_s3.hpp"

#include <cmath>

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

#include <arrow/record_batch.h>
#include <arrow/table.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <boost/format.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/irange.hpp>
#include <parquet/arrow/writer.h>

#include "aws/s3.hpp"
#include "filesystem/filesystem.hpp"
#include "operators/arrow_helpers.hpp"
#include "runtime/jit/operators/exchange_s3.hpp"
#include "runtime/jit/values/atomics.hpp"
#include "runtime/jit/values/json_parsing.hpp"
#include "runtime/jit/values/none.hpp"
#include "runtime/jit/values/tuple.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime::operators {

// NOLINTNEXTLINE(google-runtime-int)
auto ComputeGroupSizeTuple(const std::tuple<long, long, long, long> &config)
        // NOLINTNEXTLINE(google-runtime-int)
        -> std::tuple<long> {
    return ExchangeS3Operator::ComputeGroupSize(
            std::get<0>(config), std::get<1>(config), std::get<2>(config),
            std::get<3>(config));
}

auto MakeExchangeS3Operator(std::unique_ptr<ValueOperator> &&main_upstream,
                            std::unique_ptr<ValueOperator> &&dop_upstream,
                            std::unique_ptr<ValueOperator> &&wid_upstream,
                            std::vector<std::string> column_types,
                            const size_t exchange_id, const size_t num_levels,
                            const size_t level_num)
        -> std::unique_ptr<ValueOperator> {
    // Construct column names
    std::vector<std::string> column_names;
    for (size_t i = 0; i < column_types.size(); i++) {
        column_names.push_back((boost::format("v%1%") % i).str());
    }

    // Construct operator
    return std::make_unique<ExchangeS3Operator>(
            std::move(main_upstream), std::move(dop_upstream),
            std::move(wid_upstream),
            MakeArrowSchema(std::move(column_types), std::move(column_names)),
            exchange_id, num_levels, level_num);
}

auto ExchangeS3Operator::LookupQueryId() -> size_t {
    static const char *const kQueryIdVarName = "JITQ_QUERY_ID";
    auto *const query_id = std::getenv(kQueryIdVarName);
    if (query_id == nullptr) {
        throw std::runtime_error(
                (boost::format("Environment variable %1% not set.") %
                 kQueryIdVarName)
                        .str());
    }
    return std::stoll(query_id);
}

auto ExchangeS3Operator::LookupBucketName() -> std::string {
    static const char *const kBucketNameVarName =
            "JITQ_EXCHANGE_S3_BUCKET_NAME";
    auto *const bucket_name = std::getenv(kBucketNameVarName);
    if (bucket_name == nullptr) {
        throw std::runtime_error(
                (boost::format("Environment variable %1% not set.") %
                 kBucketNameVarName)
                        .str());
    }
    return bucket_name;
}

void ExchangeS3Operator::open() {
    // Read degree of parallelism
    {
        dop_upstream_->open();
        auto const ret = dop_upstream_->next();
        assert(dop_upstream_->next()->as<runtime::values::None>() != nullptr);
        dop_upstream_->close();

        auto *const tuple_value = ret->as<runtime::values::Tuple>();
        auto *const int_value =
                tuple_value->fields[0]->as<runtime::values::Int64>();

        num_workers_ = int_value->value;
    }

    // Read worker ID
    {
        wid_upstream_->open();
        auto const ret = wid_upstream_->next();
        assert(wid_upstream_->next()->as<runtime::values::None>() != nullptr);
        wid_upstream_->close();

        auto *const tuple_value = ret->as<runtime::values::Tuple>();
        auto *const int_value =
                tuple_value->fields[0]->as<runtime::values::Int64>();

        worker_id_ = int_value->value;
    }

    // Compute group members and size
    group_members_ = ComputeGroupMembers(num_levels_, level_num_, num_workers_,
                                         worker_id_),
    group_size_ =
            ComputeGroupSize(num_levels_, level_num_, num_workers_, worker_id_);

    // Open main upstream
    main_upstream_->open();
}

auto ExchangeS3Operator::next() -> std::shared_ptr<values::Value> {
    // This is the first call to next --> consume upstream
    if (pending_results_.empty() && returned_results_.empty()) {
        std::vector<std::string> group_member_strs;
        for (auto const i : group_members_) {
            group_member_strs.push_back(std::to_string(i));
        }

        // Produce our output file
        ConsumeUpstream();

        // Spawn asynchronous tasks each waiting for an input file
        for (size_t i = 0; i < group_size_; i++) {
            pending_results_.emplace_back(std::async(
                    [&](auto const i) {
                        auto const sender_id = group_members_.at(i);
                        return WaitForFile(sender_id);
                    },
                    i));
        }
    }

    // We have returned all results --> signal end-of-stream
    if (pending_results_.empty()) {
        return std::make_shared<values::None>();
    }

    const auto *const filesystem = "s3";
    auto const fs = filesystem::MakeFilesystem(filesystem);

    // Wait until (at least) one file is ready
    std::string file_name;
    while (true) {
        for (auto it = pending_results_.begin(); it != pending_results_.end();
             it++) {
            auto &future = *it;
            if (future.wait_for(std::chrono::seconds(0)) ==
                std::future_status::ready) {
                file_name = future.get();
                pending_results_.erase(it);
                break;
            }
        }

        if (!file_name.empty()) break;

        // Files not ready yet --> wait a bit before retrying
        if (pending_results_.empty()) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(500ms);
        }
    }

    auto const [it, has_inserted] = returned_results_.insert(file_name);
    assert(has_inserted);

    auto const file_path = std::make_shared<values::String>();
    auto const slice_from = std::make_shared<values::Int64>();
    auto const slice_to = std::make_shared<values::Int64>();
    auto const num_slices = std::make_shared<values::Int64>();

    for (size_t i = 0; i < group_members_.size(); i++) {
        if (group_members_.at(i) == worker_id_) {
            slice_from->value = i;
            slice_to->value = i + 1;
        }
    }

    file_path->value = file_name;
    num_slices->value = group_size_;

    auto const ret = std::make_shared<values::Tuple>();
    ret->fields = {file_path, slice_from, slice_to, num_slices};
    return ret;
}

auto ExchangeS3Operator::ComputeGroupMembers(const size_t num_levels,
                                             const size_t level_num,
                                             const size_t num_workers,
                                             const size_t worker_id)
        -> std::vector<size_t> {
    std::vector<size_t> group_members;
    if (num_levels == 1) {
        boost::copy(boost::irange(0UL, num_workers),
                    std::inserter(group_members, group_members.begin()));
    } else if (num_levels == 2 && level_num == 0) {
        const auto num_groups = static_cast<size_t>(sqrt(num_workers));
        const size_t group_id = worker_id % num_groups;
        for (size_t i = 0; i < num_workers; i++) {
            if (i % num_groups == group_id) group_members.push_back(i);
        }
    } else if (num_levels == 2 && level_num == 1) {
        const auto num_groups = static_cast<size_t>(sqrt(num_workers));
        const size_t group_id = worker_id / num_groups;
        for (size_t i = 0; i < num_workers; i++) {
            if (i / num_groups == group_id) group_members.push_back(i);
        }
    } else {
        assert(false);
    }
    return group_members;
}

auto ExchangeS3Operator::ComputeGroupSize(const size_t num_levels,
                                          const size_t level_num,
                                          const size_t num_workers,
                                          const size_t worker_id) -> size_t {
    return ComputeGroupMembers(num_levels, level_num, num_workers, worker_id)
            .size();
}

auto ExchangeS3Operator::WaitForFile(const size_t sender_id) const
        -> std::string {
    auto const file_name = (boost::format("jitq/query-%1%/exchange-%2%/"
                                          "%3%.parquet") %
                            query_id_ % exchange_id_ % sender_id)
                                   .str();

    Aws::S3::Model::HeadObjectRequest request;
    request.WithBucket(bucket_name_).WithKey(file_name);
    request.SetResponseStreamFactory(
            []() { return Aws::New<Aws::StringStream>("exchange"); });

    for (size_t j = 0; true; j++) {
        const auto outcome = s3_client_->HeadObject(request);
        if (outcome.IsSuccess()) break;

        using namespace std::chrono_literals;
        // NOLINTNEXTLINE(readability-magic-numbers)
        std::this_thread::sleep_for(100ms * (1UL << std::min(j, 2UL)));
    }

    auto const file_path =
            (boost::format("s3://%1%/%2%") % bucket_name_ % file_name).str();
    return file_path;
}

void ExchangeS3Operator::ConsumeUpstream() {
    std::unordered_map<int64_t, RecordBatches> partitions;

    while (true) {
        auto const input = main_upstream_->next();
        if (dynamic_cast<const values::None *>(input.get()) != nullptr) break;

        // Convert upstream value to arrow record batch
        auto *const tuple = input->as<values::Tuple>();
        auto const key = tuple->fields.at(0)->as<values::Int64>()->value;

        std::vector<std::shared_ptr<values::Value>> columns(
                tuple->fields.begin() + 1, tuple->fields.end());
        auto value = std::make_shared<values::Tuple>();
        value->fields = std::move(columns);

        // Append to output partition
        partitions[key].emplace_back(ConvertValueToRecordBatch(value, schema_));
    }

    // Open Parquet file writer
    const auto *const filesystem = "s3";
    auto const filename =
            (boost::format("s3://%1%/jitq/query-%2%/exchange-%3%/%4%.parquet") %
             bucket_name_ % query_id_ % exchange_id_ % worker_id_)
                    .str();

    parquet::WriterProperties::Builder properties_builder;
    properties_builder.created_by("JITQ :)");
    properties_builder.compression(parquet::Compression::SNAPPY);
    properties_builder.encoding(parquet::Encoding::BIT_PACKED);
    properties_builder.enable_dictionary();
    auto const writer_properties = properties_builder.build();

    auto const fs = filesystem::MakeFilesystem(filesystem);
    auto output_stream = fs->OpenForWrite(filename);

    std::unique_ptr<parquet::arrow::FileWriter> file_writer;
    operators::ThrowIfNotOK(parquet::arrow::FileWriter::Open(
            *schema_, arrow::default_memory_pool(), output_stream,
            writer_properties, &file_writer));

    for (size_t i = 0; i < group_size_; i++) {
        // Note: [] creates an empty vector if key not present
        auto const &record_batches = partitions[i];

        // Create arrow table from record batches
        auto maybe_table =
                arrow::Table::FromRecordBatches(schema_, record_batches);
        operators::ThrowIfNotOK(maybe_table);
        auto const table = std::move(maybe_table).ValueOrDie();
        const size_t num_rows = table->num_rows();

        // Write current file
        operators::ThrowIfNotOK(file_writer->NewRowGroup(num_rows));
        for (size_t j = 0; j < table->num_columns(); j++) {
            operators::ThrowIfNotOK(file_writer->WriteColumnChunk(
                    table->column(j), 0, num_rows));
        }
    }

    operators::ThrowIfNotOK(file_writer->Close());
    operators::ThrowIfNotOK(output_stream->Close());
}

}  // namespace runtime::operators
