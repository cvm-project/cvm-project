#ifndef DAG_DAGSPLITROWDATA_H
#define DAG_DAGSPLITROWDATA_H

#include "DAGOperator.h"

class DAGSplitRowData : public DAGOperatorBase<DAGSplitRowData> {
public:
    constexpr static const char *kName = "split_row_data";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGSPLITROWDATA_H
