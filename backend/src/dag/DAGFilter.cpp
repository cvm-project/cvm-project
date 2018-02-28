
#include "dag/DAGFilter.h"

#include "dag/DAG.h"

DAGFilter *DAGFilter::copy() {
    auto *filt = new DAGFilter();
    filt->Init(dag());
    filt->fields = fields;
    filt->read_set = read_set;
    filt->llvm_ir = llvm_ir;
    filt->output_type = output_type;
    filt->id = dag()->last_operator_id() + 1;
    dag()->addOperator(filt);
    return filt;
}
