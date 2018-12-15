#ifndef CPP_PARALLELMAPOPERATOR_H
#define CPP_PARALLELMAPOPERATOR_H

#include <cassert>
#include <list>
#include <mutex>

#include <omp.h>

#include "Optional.h"
#include "Utils.h"

template <class OutputTuple, class InnerPlanFunctor, class Upstream>
class ParallelMapOperator {
public:
    ParallelMapOperator(Upstream *const upstream,
                        const InnerPlanFunctor &inner_plan)
        : upstream_(upstream), inner_plan_(inner_plan) {}

    INLINE void open() { result_it_ = results_.begin(); }

    INLINE Optional<OutputTuple> next() {
        if (results_.empty()) MaterializeUpstream();
        while (result_it_ != results_.end()) {
            auto const ret = *(result_it_++);
            if (ret) return ret;
        }
        return {};
    }

    INLINE void close() {}

private:
    void MaterializeUpstream() {
        upstream_->open();

        std::mutex lock;
        while (const auto ret = upstream_->next()) {
            Optional<OutputTuple> *thread_result_out;
            {
                std::lock_guard<std::mutex> _(lock);
                results_.emplace_back();
                thread_result_out = &(results_.back());
            }
#pragma omp task shared(results_, lock)
            {
                const auto input_tuple = ret.value();
                auto const thread_result = inner_plan_(input_tuple);

                {
                    std::lock_guard<std::mutex> _(lock);
                    (*thread_result_out) = thread_result;
                }
            }
        }
#pragma omp taskwait

        upstream_->close();
        result_it_ = results_.begin();
    }

    Upstream *const upstream_;
    const InnerPlanFunctor &inner_plan_;
    std::list<Optional<OutputTuple>> results_{};
    typename decltype(results_)::iterator result_it_{};
};

template <class OutputTuple, class InnerPlanFunctor, class Upstream>
ParallelMapOperator<OutputTuple, InnerPlanFunctor, Upstream>
makeParallelMapOperator(Upstream *const upstream,
                        const InnerPlanFunctor &inner_plan) {
    return ParallelMapOperator<OutputTuple, InnerPlanFunctor, Upstream>(
            upstream, inner_plan);
};

#endif  // CPP_PARALLELMAPOPERATOR_H
