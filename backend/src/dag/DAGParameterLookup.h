#ifndef DAG_DAGPARAMETERLOOKUP_H
#define DAG_DAGPARAMETERLOOKUP_H

#include "DAGOperator.h"

class DAGParameterLookup : public DAGOperatorBase<DAGParameterLookup> {
public:
    constexpr static const char *kName = "parameter_lookup";
    constexpr static size_t kNumInPorts = 0;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGPARAMETERLOOKUP_H
