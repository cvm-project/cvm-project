#ifndef DAG_DAGSPLITCOLUMNDATA_H
#define DAG_DAGSPLITCOLUMNDATA_H

#include "DAGOperator.h"

class DAGSplitColumnData : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitColumnData, "split_column_data");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGSPLITCOLUMNDATA_H
