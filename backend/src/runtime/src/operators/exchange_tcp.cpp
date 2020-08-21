#include "exchange_tcp.hpp"

#include <cmath>

#include <chrono>
#include <fstream>
#include <memory>
#include <regex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include <arrow/io/memory.h>
#include <arrow/record_batch.h>
#include <arrow/table.h>
#include <arrow/type.h>
#include <boost/format.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/irange.hpp>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/file_reader.h>

#include "aws/s3.hpp"
#include "filesystem/filesystem.hpp"
#include "net/tcp/exchange_service.hpp"
#include "operators/arrow_helpers.hpp"
#include "operators/arrow_table_scan.hpp"
#include "operators/record_batch_to_value.hpp"
#include "operators/value_to_record_batch.hpp"
#include "runtime/jit/values/atomics.hpp"
#include "runtime/jit/values/json_parsing.hpp"
#include "runtime/jit/values/none.hpp"
#include "runtime/jit/values/tuple.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime::operators {

auto MakeExchangeTcpOperator(std::unique_ptr<ValueOperator> &&upstream,
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
    auto exchange_tcp_op = std::make_unique<ExchangeTcpOperator>(
            std::move(upstream),
            MakeArrowSchema(std::move(column_types), std::move(column_names)),
            exchange_id, num_levels, level_num);

    auto table_scan_op = std::make_unique<ArrowTableScanOperator>(
            std::move(exchange_tcp_op));

    return std::make_unique<RecordBatchToValueOperator>(
            std::move(table_scan_op));
}

auto tcp_num_workers() -> size_t { return tcp_hosts().size(); }

auto tcp_worker_id() -> size_t {
    static const char *const kWorkerIdVarName = "JITQ_TCP_WORKER_ID";
    auto *const worker_id = std::getenv(kWorkerIdVarName);
    if (worker_id == nullptr) {
        throw std::runtime_error(
                (boost::format("Environment variable %1% not set.") %
                 kWorkerIdVarName)
                        .str());
    }
    return std::stoll(worker_id);
}

auto tcp_hosts() -> const std::vector<std::pair<std::string, size_t>> & {
    static std::vector<std::pair<std::string, size_t>> hosts;
    static std::mutex mutex;

    if (hosts.empty()) {
        std::lock_guard<std::mutex> lock(mutex);
        if (hosts.empty()) {
            // Get path to hostfile
            static const char *const kHostfilePath = "JITQ_TCP_HOSTFILE_PATH";
            auto *const hostfile_path = std::getenv(kHostfilePath);
            if (hostfile_path == nullptr) {
                throw std::runtime_error(
                        (boost::format("Environment variable %1% not set.") %
                         kHostfilePath)
                                .str());
            }

            // Parse host file
            std::ifstream host_file(hostfile_path);
            if (!host_file.is_open()) {
                throw std::runtime_error(
                        (boost::format("Could not open '%1%'.") % hostfile_path)
                                .str());
            }
            const std::regex regex("([a-zA-Z0-9.]+):([0-9]+)");
            for (std::string line; getline(host_file, line);) {
                std::smatch match;
                if (!std::regex_match(line, match, regex)) {
                    throw std::runtime_error(
                            (boost::format("Could not parse '%1%' as "
                                           "host:port.") %
                             line)
                                    .str());
                }
                hosts.emplace_back(match[1], std::stol(match[2]));
            }
        }
    }

    return hosts;
}

/*
 * Access to the singleton service.
 *
 * The singleton instance is kept in thread-local storage of the thread that
 * first accesses it. That threads makes the instance available to other
 * threads through a (global) static weak pointer. This guarantees that the
 * same thread destructs the instance that has constructed it, which is
 * necessary to avoid race conditions in the destructor.
 */
auto exchange_service() -> std::shared_ptr<net::tcp::ExchangeService> {
    static std::weak_ptr<net::tcp::ExchangeService> service;

    std::shared_ptr<net::tcp::ExchangeService> ret = service.lock();
    if (!ret) {
        thread_local std::shared_ptr<net::tcp::ExchangeService> singleton =
                std::make_unique<net::tcp::ExchangeService>(tcp_hosts(),
                                                            tcp_worker_id());
        service = ret = singleton;
    }

    return ret;
}

void ExchangeTcpOperator::ConsumeUpstream() {
    // Set up Parquet writer properties
    parquet::WriterProperties::Builder properties_builder;
    properties_builder.created_by("JITQ :)");
    properties_builder.compression(parquet::Compression::SNAPPY);
    properties_builder.encoding(parquet::Encoding::BIT_PACKED);
    properties_builder.enable_dictionary();
    auto const writer_properties = properties_builder.build();

    while (true) {
        auto const input = upstream_->next();
        if (dynamic_cast<const values::None *>(input.get()) != nullptr) break;

        // Convert upstream value to arrow table
        auto *const tuple = input->as<values::Tuple>();
        auto const key = tuple->fields.at(0)->as<values::Int64>()->value;

        std::vector<std::shared_ptr<values::Value>> columns(
                tuple->fields.begin() + 1, tuple->fields.end());
        auto value = std::make_shared<values::Tuple>();
        value->fields = std::move(columns);

        auto const record_batch = ConvertValueToRecordBatch(value, schema_);

        auto const maybe_table =
                arrow::Table::FromRecordBatches(schema_, {record_batch});
        operators::ThrowIfNotOK(maybe_table);
        auto const &table = maybe_table.ValueOrDie();

        const size_t num_rows = table->num_rows();

        // Open Parquet file writer
        auto const maybe_output_stream =
                arrow::io::BufferOutputStream::Create(1UL << 14U);
        operators::ThrowIfNotOK(maybe_output_stream);
        auto const &output_stream = maybe_output_stream.ValueOrDie();

        std::unique_ptr<parquet::arrow::FileWriter> file_writer;
        operators::ThrowIfNotOK(parquet::arrow::FileWriter::Open(
                *schema_, arrow::default_memory_pool(), output_stream,
                writer_properties, &file_writer));

        // Write table to Parquet file writer
        operators::ThrowIfNotOK(file_writer->NewRowGroup(num_rows));
        for (size_t i = 0; i < table->num_columns(); i++) {
            operators::ThrowIfNotOK(file_writer->WriteColumnChunk(
                    table->column(i), 0, num_rows));
        }

        // Finalize Parquet writer and underlying buffer
        operators::ThrowIfNotOK(file_writer->Close());

        auto const maybe_buffer = output_stream->Finish();
        operators::ThrowIfNotOK(maybe_buffer);
        auto const &buffer = maybe_buffer.ValueOrDie();

        // Send to remote side
        // XXX: Buffer::ToString makes a copy!
        exchange_service()->SendMessage(exchange_id_, group_members_.at(key),
                                        buffer->ToString());
    }

    // End of upstream, signal end-of-stream to other workers
    for (auto const receiver_id : group_members_) {
        exchange_service()->SendMessage(exchange_id_, receiver_id, {});
    }
}

void ExchangeTcpOperator::open() {
    // XXX: ExchangeService does not support varying subset of hosts
    assert(group_members_.size() == tcp_num_workers());

    upstream_->open();
    exchange_service()->StartExchange(exchange_id_);
}

auto ExchangeTcpOperator::next() -> std::shared_ptr<arrow::Table> {
    if (!has_consume_upstream_started_) {
        has_consume_upstream_started_ = true;
        consume_upstream_thread_ = std::thread([this]() { ConsumeUpstream(); });
    }

    // Receive next message
    auto message = exchange_service()->ReceiveMessage(exchange_id_);

    if (!message) {  // No more messages from other workers,
        return {};   // return end-of-stream
    }

    // Wrap message into buffer
    auto const buffer = arrow::Buffer::FromString(std::move(message.value()));

    // Open buffer as Parquet file
    auto const source = std::make_shared<arrow::io::BufferReader>(buffer);
    std::unique_ptr<parquet::arrow::FileReader> arrow_reader;
    auto file_reader = parquet::arrow::OpenFile(
            source, arrow::default_memory_pool(), &arrow_reader);

    // Read Parquet file as Arrow table
    std::shared_ptr<arrow::Table> table;
    operators::ThrowIfNotOK(arrow_reader->ReadTable(&table));

    return table;
}

void ExchangeTcpOperator::close() {
    consume_upstream_thread_.join();
    exchange_service()->FinishExchange(exchange_id_);
    upstream_->close();
}

}  // namespace runtime::operators
