#ifndef OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP
#define OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP

#include <memory>
#include <string>
#include <vector>

#include "runtime/jit/operators/value_operator.hpp"
#include "runtime/jit/values/value.hpp"

namespace runtime {
namespace operators {

class ConcurrentExecuteLambdaOperator : public ValueOperator {
public:
    ConcurrentExecuteLambdaOperator(std::unique_ptr<ValueOperator>&& upstream,
                                    std::string inner_dag)
        : upstream_(std::move(upstream)), inner_dag_(std::move(inner_dag)) {}
    void open() override;
    auto next() -> std::shared_ptr<runtime::values::Value> override;
    void close() override;

private:
    void MaterializeResult();

    std::unique_ptr<ValueOperator> upstream_;
    std::string inner_dag_;
    std::vector<std::shared_ptr<values::Value>> results_;
    typename decltype(results_)::iterator result_it_{};
};

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_CONCURRENT_EXECUTE_LAMBDA_HPP
