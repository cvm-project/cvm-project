#ifndef OPERATORS_EXCHANGE_S3_HPP
#define OPERATORS_EXCHANGE_S3_HPP

#include <future>
#include <list>
#include <memory>
#include <thread>
#include <unordered_set>
#include <vector>

#include <arrow/record_batch.h>
#include <arrow/type.h>
#include <aws/s3/S3Client.h>

#include "aws/s3.hpp"
#include "operators/value_to_record_batch.hpp"
#include "runtime/jit/operators/value_operator.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace operators {

class ExchangeS3Operator : public ValueOperator {
public:
    ExchangeS3Operator(std::unique_ptr<ValueOperator> &&main_upstream,
                       std::unique_ptr<ValueOperator> &&dop_upstream,
                       std::unique_ptr<ValueOperator> &&wid_upstream,
                       std::shared_ptr<arrow::Schema> schema,
                       const size_t exchange_id, const size_t num_levels,
                       const size_t level_num)
        : main_upstream_(std::move(main_upstream)),
          dop_upstream_(std::move(dop_upstream)),
          wid_upstream_(std::move(wid_upstream)),
          schema_(std::move(schema)),
          bucket_name_(LookupBucketName()),
          s3_client_(aws::s3::MakeClient()),
          query_id_(LookupQueryId()),
          exchange_id_(exchange_id),
          num_levels_(num_levels),
          level_num_(level_num) {}

    void open() override;
    auto next() -> std::shared_ptr<values::Value> override;
    void close() override { main_upstream_->close(); }

private:
    using RecordBatches = std::vector<std::shared_ptr<arrow::RecordBatch>>;

    static auto LookupQueryId() -> size_t;
    static auto LookupBucketName() -> std::string;
    static auto ComputeGroupMembers(size_t num_levels, size_t level_num,
                                    size_t num_workers, size_t worker_id)
            -> std::vector<size_t>;
    static auto ComputeGroupSize(size_t num_levels, size_t level_num,
                                 size_t num_workers, size_t worker_id)
            -> size_t;

    // cppcheck-suppress unusedPrivateFunction  // false positive
    void ConsumeUpstream();
    // cppcheck-suppress unusedPrivateFunction  // false positive
    auto WaitForFile(size_t sender_id) const -> std::string;

    const std::unique_ptr<ValueOperator> main_upstream_;
    const std::unique_ptr<ValueOperator> dop_upstream_;
    const std::unique_ptr<ValueOperator> wid_upstream_;
    const std::shared_ptr<arrow::Schema> schema_;
    const std::string bucket_name_;
    std::shared_ptr<Aws::S3::S3Client> s3_client_;
    const size_t query_id_;
    const size_t exchange_id_;
    const size_t num_levels_;
    const size_t level_num_;
    size_t num_workers_{};
    size_t worker_id_{};
    std::vector<size_t> group_members_{};
    size_t group_size_{};
    std::list<std::future<std::string>> pending_results_;
    std::unordered_set<std::string> returned_results_;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_EXCHANGE_S3_HPP
