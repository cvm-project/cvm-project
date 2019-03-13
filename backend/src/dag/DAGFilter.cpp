
#include "DAGFilter.h"

DAGFilter *DAGFilter::copy() {
    std::unique_ptr<DAGFilter> filt(new DAGFilter());
    filt->tuple = tuple;
    filt->read_set = read_set;
    filt->llvm_ir = llvm_ir;
    return filt.release();
}
