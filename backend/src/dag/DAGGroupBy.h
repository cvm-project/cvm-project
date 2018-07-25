#ifndef DAG_DAGGROUPBY_H
#define DAG_DAGGROUPBY_H

#include "DAGOperator.h"

class DAGGroupBy : public DAGOperatorBase<DAGGroupBy> {
public:
    constexpr static const char *kName = "groupby";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGGROUPBY_H
