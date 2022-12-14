//
// Created by sabir on 29.08.17.
//

#include "simple_predicate_move_around.hpp"

#include <algorithm>
#include <unordered_set>
#include <vector>

#include <boost/mpl/list.hpp>
#include <polymorphic_value.h>

#include "dag/dag.hpp"
#include "dag/operators/filter.hpp"
#include "llvm_helpers/function.hpp"
#include "utils/visitor.hpp"

struct CollectFiltersVisitor
    : public Visitor<CollectFiltersVisitor, DAGOperator,
                     boost::mpl::list<DAGFilter>> {
    void operator()(DAGFilter *op) { filters_.emplace_back(op); }
    std::vector<DAGFilter *> filters_;
};

namespace optimize {

void SimplePredicateMoveAround::Run(DAG *const dag,
                                    const std::string & /*config*/) const {
    // 1. gather filters
    CollectFiltersVisitor filter_collector;
    for (auto *const op : dag->operators()) {
        filter_collector.Visit(op);
    }

    // 2. for every filter move them up the dag
    for (const auto &filter : filter_collector.filters_) {
        // Go as high up as the filter could go
        DAGOperator *tip = filter;
        while (dag->out_degree(tip) == 1 &&
               std::all_of(filter->read_set.begin(), filter->read_set.end(),
                           [&](auto c) {
                               return dag->successor(tip)->HasInOutput(c.get());
                           })) {
            tip = dag->successor(tip);
        }

        // From the tip, push the filter down as far as possible
        std::vector<DAGOperator *> q;
        std::unordered_set<DAGOperator *> new_predecessors;
        q.push_back(tip);
        while (!q.empty()) {
            DAGOperator *currentOp = q.back();
            q.pop_back();

            bool can_swap = false;
            for (auto const flow : dag->in_flows(currentOp)) {
                auto *const pred = flow.source.op;
                if (currentOp->name() != "row_scan" &&
                    currentOp->name() != "range_source" &&
                    std::all_of(filter->read_set.begin(),
                                filter->read_set.end(), [&](auto c) {
                                    return pred->HasInOutput(c.get());
                                })) {
                    // The predecessor can still read all fields
                    // --> push it further
                    q.push_back(pred);
                    can_swap = true;
                }
            }

            // We could push it beyond some predecessor
            // --> do not add filter here
            if (can_swap) {
                continue;
            }

            // Filter will be added after currentOp

            assert(std::all_of(
                    filter->read_set.begin(), filter->read_set.end(),
                    [&](auto c) { return currentOp->HasInOutput(c.get()); }));

            new_predecessors.insert(currentOp);
        }

        auto *const pred = dag->predecessor(filter);
        const bool keep_original = new_predecessors.count(pred) > 0;
        new_predecessors.erase(pred);

        // Bypass the filter, i.e., connect the predecessors and successors if
        // this is the sink, reassign the sink
        if (!keep_original) {
            const auto in_flow = dag->in_flow(filter);
            const auto out_flow = dag->out_flow(filter);

            dag->RemoveFlow(in_flow);
            dag->RemoveFlow(out_flow);

            dag->AddFlow(in_flow.source.op, in_flow.source.port,
                         out_flow.target.op, out_flow.target.port);
        }

        // Add copy of the filter into the new places
        for (auto *const currentOp : new_predecessors) {
            DAGFilter *const filt = filter->Clone();
            dag->AddOperator(filt);

            // insert the filter after this operator
            const auto out_flow = dag->out_flow(currentOp, 0);
            dag->RemoveFlow(out_flow);

            dag->AddFlow(filt, 0, out_flow.target.op, out_flow.target.port);
            dag->AddFlow(currentOp, 0, filt, 0);

            // change the llvm ir signature
            llvm_helpers::Function func(filt->llvm_ir);
            func.AdjustFilterSignature(filt, dag->predecessor(filt));
            filt->llvm_ir = func.str();

            // copy the tuple
            filt->tuple = isocpp_p0201::make_polymorphic_value<
                    dag::collection::Tuple>(*currentOp->tuple);
        }

        // Remove the filter
        if (!keep_original) {
            dag->RemoveOperator(filter);
        }
    }
}

}  // namespace optimize
