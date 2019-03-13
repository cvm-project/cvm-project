#ifndef DAG_DAGGROUPBY_H
#define DAG_DAGGROUPBY_H

#include "DAGOperator.h"

class DAGGroupBy : public DAGOperator {
    JITQ_DAGOPERATOR(DAGGroupBy, "groupby");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGGROUPBY_H
