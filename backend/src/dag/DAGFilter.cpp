
#include "dag/DAGFilter.h"

#include <memory>

DAGFilter *DAGFilter::copy() {
    std::unique_ptr<DAGFilter> filt(new DAGFilter());
    filt->fields = fields;
    filt->read_set = read_set;
    filt->llvm_ir = llvm_ir;
    filt->output_type = output_type;
    return filt.release();
}
