//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGJOIN_H
#define DAG_DAGJOIN_H

#include "DAGOperator.h"

class DAGJoin : public DAGOperator {
    JITQ_DAGOPERATOR(DAGJoin, "join");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGJOIN_H
