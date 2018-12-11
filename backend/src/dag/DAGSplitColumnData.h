#ifndef DAG_DAGSPLITCOLUMNDATA_H
#define DAG_DAGSPLITCOLUMNDATA_H

#include "DAGOperator.h"

class DAGSplitColumnData : public DAGOperatorBase<DAGSplitColumnData> {
public:
    constexpr static const char *kName = "split_column_data";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGSPLITCOLUMNDATA_H
