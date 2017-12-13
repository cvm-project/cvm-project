//
// Created by sabir on 29.08.17.
//

#include "SimplePredicateMoveAround.h"

#include <vector>

#include "IR_analyzer/LLVMParser.h"
#include "dag/DAGFilter.h"

void SimplePredicateMoveAround::optimize(DAG *dag_) {
    // 1. gather filters and remove from dag
    dag = dag_;
    dag->sink->accept(this);
    // 2. for every filter go up the dag and insert them

    for (auto filter : filters) {
        std::vector<DAGOperator *> q;
        q.push_back(dag->sink);
        while (!q.empty()) {
            DAGOperator *currentOp = q.back();
            q.pop_back();
            for (auto pred : currentOp->predecessors) {
                q.push_back(pred);
            }
            // check if at least one of filter's reads is in this op's write set
            for (auto c : filter->read_set) {
                if (currentOp->writeSetContains(c)) {
                    DAGFilter *filt = filter->copy();
                    // insert the filter after this operator
                    filt->predecessors.push_back(currentOp);
                    filt->successors = currentOp->successors;

                    if (!currentOp->successors.empty()) {
                        currentOp->successors[0] = filt;
                        DAGOperator *succ = filt->successors[0];

                        ptrdiff_t index = distance(
                                succ->predecessors.begin(),
                                find(succ->predecessors.begin(),
                                     succ->predecessors.end(), currentOp));
                        succ->predecessors[index] = filt;
                    } else {
                        // currentOp must be the sink
                        // reassign sink to the filter
                        dag->sink = filt;
                        currentOp->successors.push_back(filt);
                    }

                    // change the llvm ir signature
                    LLVMParser parser(filt->llvm_ir);
                    filt->llvm_ir = parser.adjust_filter_signature(filt);

                    // copy the output fields
                    filt->fields.clear();
                    for (auto f : filt->predecessors[0]->fields) {
                        filt->fields.push_back(f);
                    }
                    filt->output_type = filt->predecessors[0]->output_type;

                    break;
                }
            }
        }
    }
    for (auto filter : filters) {
        filter->freeThisOperator();
    }
}

void SimplePredicateMoveAround::visit(DAGFilter *op) {
    DAGVisitor::visit(op);
    DAGFilter *filt = op->copy();
    DAGOperator *pred = op->predecessors[0];

    // connect the predecessors and successors
    // if this is the sink, reassign the sink
    if (op->successors.empty()) {
        pred->successors.clear();
        dag->sink = pred;
    } else {
        DAGOperator *succ = op->successors[0];
        pred->successors[0] = succ;

        ptrdiff_t index = distance(
                succ->predecessors.begin(),
                find(succ->predecessors.begin(), succ->predecessors.end(), op));
        succ->predecessors[index] = pred;
    }
    op->freeThisOperator();
    filters.push_back(filt);
}
