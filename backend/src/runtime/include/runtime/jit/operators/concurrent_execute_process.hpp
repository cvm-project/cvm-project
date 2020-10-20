#ifndef RUNTIME_JIT_OPERATORS_CONCURRENT_EXECUTE_PROCESS_HPP
#define RUNTIME_JIT_OPERATORS_CONCURRENT_EXECUTE_PROCESS_HPP

#include <memory>
#include <string>

#include "runtime/jit/operators/value_operator.hpp"

namespace runtime {
namespace operators {

auto process_num_workers() -> size_t;
auto process_worker_id() -> size_t;

auto MakeConcurrentExecuteProcessOperator(
        std::unique_ptr<ValueOperator>&& upstream, std::string inner_plan)
        -> std::unique_ptr<ValueOperator>;

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_CONCURRENT_EXECUTE_PROCESS_HPP
