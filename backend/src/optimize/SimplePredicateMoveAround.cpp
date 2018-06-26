//
// Created by sabir on 29.08.17.
//

#include "SimplePredicateMoveAround.h"

#include <algorithm>
#include <unordered_set>
#include <vector>

#include <boost/mpl/list.hpp>

#include "dag/DAGFilter.h"
#include "llvm_helpers/function.hpp"
#include "utils/visitor.hpp"

struct CollectFiltersVisitor
    : public Visitor<CollectFiltersVisitor, DAGOperator,
                     boost::mpl::list<DAGFilter>> {
    void operator()(DAGFilter *op) { filters_.emplace_back(op); }
    std::vector<DAGFilter *> filters_;
};

void SimplePredicateMoveAround::optimize() {
    // 1. gather filters
    CollectFiltersVisitor filter_collector;
    for (auto const op : dag_->operators()) {
        filter_collector.Visit(op);
    }

    // 2. for every filter move them up the dag
    for (const auto &filter : filter_collector.filters_) {
        // Go as high up as the filter could go
        DAGOperator *tip = filter;
        while (dag_->out_degree(tip) == 1 &&
               std::all_of(filter->read_set.begin(), filter->read_set.end(),
                           [&](auto c) {
                               return dag_->successor(tip)->HasInOutput(
                                       c.get());
                           })) {
            tip = dag_->successor(tip);
        }

        // From the tip, push the filter down as far as possible
        std::vector<DAGOperator *> q;
        std::unordered_set<DAGOperator *> new_predecessors;
        q.push_back(tip);
        while (!q.empty()) {
            DAGOperator *currentOp = q.back();
            q.pop_back();

            bool can_swap = false;
            for (auto const flow : dag_->in_flows(currentOp)) {
                auto const pred = flow.source;
                if (currentOp->name() != "collection_source" &&
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

        const auto pred = dag_->predecessor(filter);
        const bool keep_original = new_predecessors.count(pred) > 0;
        new_predecessors.erase(pred);

        // Bypass the filter, i.e., connect the predecessors and successors if
        // this is the sink, reassign the sink
        if (!keep_original) {
            const auto in_flow = dag_->in_flow(filter);
            dag_->RemoveFlow(in_flow);

            if (filter == dag_->sink) {
                dag_->sink = in_flow.source;
            } else {
                const auto out_flow = dag_->out_flow(filter);
                dag_->RemoveFlow(out_flow);
                dag_->AddFlow(in_flow.source, in_flow.source_port,
                              out_flow.target, out_flow.target_port);
            }
        }

        // Add copy of the filter into the new places
        for (auto const currentOp : new_predecessors) {
            DAGFilter *filt = filter->copy();
            dag_->AddOperator(filt);
            // insert the filter after this operator
            if (currentOp == dag_->sink) {
                // reassign sink to the filter
                dag_->sink = filt;
            } else {
                const auto out_flow = dag_->out_flow(currentOp, 0);
                dag_->RemoveFlow(out_flow);
                dag_->AddFlow(filt, 0, out_flow.target, out_flow.target_port);
            }
            dag_->AddFlow(currentOp, 0, filt, 0);

            // change the llvm ir signature
            llvm_helpers::Function parser(filt->llvm_ir);
            filt->llvm_ir = parser.adjust_filter_signature(
                    filt, dag_->predecessor(filt));

            // copy the output fields
            filt->tuple->fields.clear();
            for (const auto &f : currentOp->tuple->fields) {
                filt->tuple->fields.push_back(f);
            }
            filt->tuple->type = currentOp->tuple->type;
        }

        // Remove the filter
        if (!keep_original) {
            dag_->RemoveOperator(filter);
        }
    }
}
