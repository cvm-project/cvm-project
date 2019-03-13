//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCEBYKEY_H
#define DAG_DAGREDUCEBYKEY_H

#include "DAGOperator.h"

class DAGReduceByKey : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduceByKey, "reduce_by_key");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGREDUCEBYKEY_H
