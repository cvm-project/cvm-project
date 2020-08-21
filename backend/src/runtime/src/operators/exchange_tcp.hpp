#ifndef OPERATORS_EXCHANGE_TCP_HPP
#define OPERATORS_EXCHANGE_TCP_HPP

#include <memory>
#include <thread>
#include <vector>

#include <arrow/type.h>

#include "aws/s3.hpp"
#include "operators/arrow_table_operator.hpp"
#include "operators/exchange_s3.hpp"
#include "runtime/jit/operators/value_operator.hpp"

namespace runtime {
namespace operators {

auto tcp_num_workers() -> size_t;
auto tcp_worker_id() -> size_t;
auto tcp_hosts() -> const std::vector<std::pair<std::string, size_t>> &;

struct ExchangeTcpOperator : public ArrowTableOperator {
    ExchangeTcpOperator(std::unique_ptr<ValueOperator> &&upstream,
                        std::shared_ptr<arrow::Schema> schema,
                        const size_t exchange_id, const size_t num_levels,
                        const size_t level_num)
        : upstream_(std::move(upstream)),
          schema_(std::move(schema)),
          exchange_id_(exchange_id),
          num_workers_(tcp_num_workers()),
          worker_id_(tcp_worker_id()),
          group_members_(ExchangeS3Operator::ComputeGroupMembers(
                  num_levels, level_num, num_workers_, worker_id_)),
          group_size_(group_members_.size()) {}

    void open() override;
    auto next() -> std::shared_ptr<arrow::Table> override;
    void close() override;

private:
    // cppcheck-suppress unusedPrivateFunction  // false positive
    void ConsumeUpstream();

    const std::unique_ptr<ValueOperator> upstream_;
    const std::shared_ptr<arrow::Schema> schema_;
    const size_t exchange_id_;
    const size_t num_workers_;
    const size_t worker_id_;
    std::vector<size_t> group_members_;
    const size_t group_size_;
    std::thread consume_upstream_thread_;
    bool has_consume_upstream_started_ = false;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_EXCHANGE_TCP_HPP
