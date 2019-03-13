#ifndef DAG_DAGSPLITROWDATA_H
#define DAG_DAGSPLITROWDATA_H

#include "DAGOperator.h"

class DAGSplitRowData : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitRowData, "split_row_data");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGSPLITROWDATA_H
