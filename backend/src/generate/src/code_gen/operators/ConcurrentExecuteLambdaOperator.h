#ifndef CODE_GEN_OPERATORS_CONCURRENTEXECUTELAMBDAOPERATOR_H
#define CODE_GEN_OPERATORS_CONCURRENTEXECUTELAMBDAOPERATOR_H

#include "TupleToValueOperator.h"
#include "runtime/jit/operators/concurrent_execute_lambda.hpp"
#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/values/none.hpp"

template <class OutputTuple, class Upstream>
class ConcurrentExecuteLambdaOperator {
public:
    ConcurrentExecuteLambdaOperator(Upstream *const upstream,
                                    std::string inner_plan)
        : upstream_(runtime::operators::MakeConcurrentExecuteLambdaOperator(
                  std::make_unique<TupleToValueOperator<Upstream>>(upstream),
                  std::move(inner_plan))) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<OutputTuple> next() {
        const auto ret = upstream_->next();
        if (dynamic_cast<const runtime::values::None *>(ret.get()) != nullptr) {
            return {};
        }
        return ValueToTuple<OutputTuple>(ret);
    }

    INLINE void close() { upstream_->close(); }

private:
    std::unique_ptr<runtime::operators::ValueOperator> upstream_;
};

template <class OutputTuple, class Upstream>
ConcurrentExecuteLambdaOperator<OutputTuple, Upstream>
makeConcurrentExecuteLambdaOperator(Upstream *const upstream,
                                    std::string inner_plan) {
    return ConcurrentExecuteLambdaOperator<OutputTuple, Upstream>(
            upstream, std::move(inner_plan));
};

#endif  // CODE_GEN_OPERATORS_CONCURRENTEXECUTELAMBDAOPERATOR_H
