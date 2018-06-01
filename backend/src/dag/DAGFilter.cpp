
#include "dag/DAGFilter.h"

using dag::collection::Tuple;
DAGFilter *DAGFilter::copy() {
    std::unique_ptr<DAGFilter> filt(new DAGFilter());
    std::shared_ptr<Tuple> out(new Tuple(*tuple));
    filt->tuple = out;
    filt->read_set = read_set;
    filt->llvm_ir = llvm_ir;
    return filt.release();
}
