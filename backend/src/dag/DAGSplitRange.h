#ifndef DAG_DAGSPLITRANGE_H
#define DAG_DAGSPLITRANGE_H

#include "DAGOperator.h"

class DAGSplitRange : public DAGOperatorBase<DAGSplitRange> {
public:
    constexpr static const char *kName = "split_range";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGSPLITRANGE_H
