#ifndef DAG_DAGPARAMETERLOOKUP_H
#define DAG_DAGPARAMETERLOOKUP_H

#include "DAGOperator.h"

class DAGParameterLookup : public DAGOperator {
    JITQ_DAGOPERATOR(DAGParameterLookup, "parameter_lookup");

public:
    size_t num_in_ports() const override { return 0; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGPARAMETERLOOKUP_H
