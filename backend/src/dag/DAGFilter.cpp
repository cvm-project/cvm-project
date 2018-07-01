
#include "dag/DAGFilter.h"

using dag::collection::Tuple;
DAGFilter *DAGFilter::copy() {
    std::unique_ptr<DAGFilter> filt(new DAGFilter());
    filt->tuple = std::make_unique<Tuple>(*tuple);
    filt->read_set = read_set;
    filt->llvm_ir = llvm_ir;
    return filt.release();
}
