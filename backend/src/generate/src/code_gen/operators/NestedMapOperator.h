#ifndef CODE_GEN_OPERATORS_NESTEDMAPOPERATOR_H
#define CODE_GEN_OPERATORS_NESTEDMAPOPERATOR_H

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class OutputTuple, class InnerPlanFunctor, class Upstream>
class NestedMapOperator {
public:
    NestedMapOperator(Upstream *const upstream,
                      const InnerPlanFunctor &inner_plan)
        : upstream_(upstream), inner_plan_(inner_plan) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<OutputTuple> next() {
        const auto ret = upstream_->next();
        if (!ret) return {};
        return inner_plan_(ret.value());
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    const InnerPlanFunctor &inner_plan_;
};

template <class OutputTuple, class InnerPlanFunctor, class Upstream>
NestedMapOperator<OutputTuple, InnerPlanFunctor, Upstream>
makeNestedMapOperator(Upstream *const upstream,
                      const InnerPlanFunctor &inner_plan) {
    return NestedMapOperator<OutputTuple, InnerPlanFunctor, Upstream>(
            upstream, inner_plan);
};

#endif  // CODE_GEN_OPERATORS_NESTEDMAPOPERATOR_H
