#ifndef RUNTIME_JIT_OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP
#define RUNTIME_JIT_OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP

#include <memory>
#include <string>

#include "runtime/jit/operators/value_operator.hpp"

namespace runtime {
namespace operators {

auto lambda_num_workers() -> size_t;
auto lambda_worker_id() -> size_t;

auto MakeConcurrentExecuteLambdaOperator(
        std::unique_ptr<ValueOperator>&& upstream, std::string inner_plan)
        -> std::unique_ptr<ValueOperator>;

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP
