#ifndef OPERATORS_RECORD_BATCH_OPERATOR_HPP
#define OPERATORS_RECORD_BATCH_OPERATOR_HPP

#include <memory>

namespace arrow {
class RecordBatch;
}  // namespace arrow

namespace runtime {
namespace operators {

struct RecordBatchOperator {
    virtual void open() {}
    virtual auto next() -> std::shared_ptr<arrow::RecordBatch> = 0;
    virtual void close() {}
    virtual ~RecordBatchOperator() = default;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_RECORD_BATCH_OPERATOR_HPP
