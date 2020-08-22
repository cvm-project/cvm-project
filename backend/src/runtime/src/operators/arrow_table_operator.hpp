#ifndef OPERATORS_ARROW_TABLE_OPERATOR_HPP
#define OPERATORS_ARROW_TABLE_OPERATOR_HPP

#include <memory>

namespace arrow {
class Table;
}  // namespace arrow

namespace runtime {
namespace operators {

struct ArrowTableOperator {
    virtual void open() {}
    virtual auto next() -> std::shared_ptr<arrow::Table> = 0;
    virtual void close() {}
    virtual ~ArrowTableOperator() = default;
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_ARROW_TABLE_OPERATOR_HPP
