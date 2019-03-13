#ifndef DAG_DAGSPLITRANGE_H
#define DAG_DAGSPLITRANGE_H

#include "DAGOperator.h"

class DAGSplitRange : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitRange, "split_range");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGSPLITRANGE_H
