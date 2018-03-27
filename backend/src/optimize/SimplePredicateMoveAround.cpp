//
// Created by sabir on 29.08.17.
//

#include "SimplePredicateMoveAround.h"

#include <vector>

#include "IR_analyzer/LLVMParser.h"
#include "dag/DAGFilter.h"
#include "utils/DAGVisitor.h"

class CollectFiltersVisitor : public DAGVisitor {
public:
    explicit CollectFiltersVisitor(DAG *const dag) : DAGVisitor(dag) {}
    void visit(DAGFilter *op) override { filters_.emplace_back(op); }
    std::vector<DAGFilter *> filters_;
};

void SimplePredicateMoveAround::optimize() {
    // 1. gather filters
    CollectFiltersVisitor filter_collector(dag_);
    filter_collector.StartVisit();

    // 2. for every filter move them up the dag
    for (const auto &filter : filter_collector.filters_) {
        // 2a. bypass the filter, i.e., connect the predecessors and successors
        //     if this is the sink, reassign the sink
        const auto in_flow = dag_->in_flow(filter);
        dag_->RemoveFlow(in_flow);

        if (filter == dag_->sink) {
            dag_->sink = in_flow.source;
        } else {
            const auto out_flow = dag_->out_flow(filter);
            dag_->RemoveFlow(out_flow);
            dag_->AddFlow(in_flow.source, in_flow.source_port, out_flow.target,
                          out_flow.target_port);
        }

        // 2b. add the filter up in the dag
        std::vector<DAGOperator *> q;
        q.push_back(dag_->sink);
        while (!q.empty()) {
            DAGOperator *currentOp = q.back();
            q.pop_back();
            for (const auto &in_flow : dag_->in_flows(currentOp)) {
                q.push_back(in_flow.source);
            }
            // check if at least one of filter's reads is in this op's write set
            for (auto c : filter->read_set) {
                if (currentOp->writeSetContains(c)) {
                    DAGFilter *filt = filter->copy();
                    dag_->AddOperator(filt);
                    // insert the filter after this operator
                    if (currentOp == dag_->sink) {
                        // reassign sink to the filter
                        dag_->sink = filt;
                    } else {
                        const auto out_flow = dag_->out_flow(currentOp, 0);
                        dag_->RemoveFlow(out_flow);
                        dag_->AddFlow(filt, 0, out_flow.target,
                                      out_flow.target_port);
                    }
                    dag_->AddFlow(currentOp, 0, filt, 0);

                    // change the llvm ir signature
                    LLVMParser parser(filt->llvm_ir);
                    filt->llvm_ir = parser.adjust_filter_signature(
                            filt, dag_->predecessor(filt));

                    // copy the output fields
                    filt->fields.clear();
                    for (const auto &f : currentOp->fields) {
                        filt->fields.push_back(f);
                    }
                    filt->output_type = currentOp->output_type;

                    break;
                }
            }
        }

        // 2c. remove the filter
        dag_->RemoveOperator(filter);
    }
}
