#ifndef RUNTIME_JIT_OPERATORS_EXCHANGE_S3_HPP
#define RUNTIME_JIT_OPERATORS_EXCHANGE_S3_HPP

#include <memory>
#include <string>
#include <vector>

#include "runtime/jit/operators/value_operator.hpp"

namespace runtime {
namespace operators {

std::unique_ptr<ValueOperator> MakeExchangeS3Operator(
        std::unique_ptr<ValueOperator> &&main_upstream,
        std::unique_ptr<ValueOperator> &&dop_upstream,
        std::unique_ptr<ValueOperator> &&wid_upstream,
        std::vector<std::string> column_types, const size_t exchange_id,
        const size_t num_levels, const size_t level_num);

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_EXCHANGE_S3_HPP
