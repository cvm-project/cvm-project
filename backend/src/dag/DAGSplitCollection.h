#ifndef DAG_DAGSPLITCOLLECTION_H
#define DAG_DAGSPLITCOLLECTION_H

#include "DAGOperator.h"

class DAGSplitCollection : public DAGOperatorBase<DAGSplitCollection> {
public:
    constexpr static const char *kName = "split_collection";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGSPLITCOLLECTION_H
