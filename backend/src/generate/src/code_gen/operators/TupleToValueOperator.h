#ifndef CODE_GEN_CPP_OPERATORS_TUPLETOVALUEOPERATOR_H
#define CODE_GEN_CPP_OPERATORS_TUPLETOVALUEOPERATOR_H

#include <memory>

#include "runtime/jit/operators/value_operator.hpp"
#include "runtime/jit/values/value.hpp"

template <class Upstream>
struct TupleToValueOperator : runtime::operators::ValueOperator {
    TupleToValueOperator(Upstream *const upstream) : upstream_(upstream) {}

    void open() override { upstream_->open(); }

    std::shared_ptr<runtime::values::Value> next() override {
        return TupleToValue(upstream_->next());
    }

    void close() override { upstream_->close(); }

private:
    Upstream *const upstream_;
};

#endif  // CODE_GEN_CPP_OPERATORS_TUPLETOVALUEOPERATOR_H
