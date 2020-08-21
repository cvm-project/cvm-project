#ifndef RUNTIME_JIT_OPERATORS_EXCHANGE_TCP_HPP
#define RUNTIME_JIT_OPERATORS_EXCHANGE_TCP_HPP

#include <memory>
#include <string>
#include <vector>

#include "runtime/jit/operators/value_operator.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace operators {

std::unique_ptr<ValueOperator> MakeExchangeTcpOperator(
        std::unique_ptr<ValueOperator> &&upstream,
        std::vector<std::string> column_types, const size_t exchange_id,
        const size_t num_levels, const size_t level_num);

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_EXCHANGE_TCP_HPP
