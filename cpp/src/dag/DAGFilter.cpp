
#include "dag/DAGFilter.h"

#include "dag/DAG.h"
#include "utils/DAGVisitor.h"

void DAGFilter::accept(DAGVisitor *v) { v->visit(this); }

DAGFilter *DAGFilter::copy() {
    auto *filt = new DAGFilter(dag_);
    filt->fields = fields;
    filt->read_set = read_set;
    filt->llvm_ir = llvm_ir;
    filt->output_type = output_type;
    filt->id = ++(dag_->last_operator_id);
    return filt;
}
