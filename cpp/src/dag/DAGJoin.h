//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGJOIN_H
#define DAG_DAGJOIN_H

#include <iostream>

#include "DAGOperator.h"

class DAGJoin : public DAGOperator {
public:
    explicit DAGJoin(DAG *const dag) : DAGOperator(dag) {}

    bool stream_right = true;
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGJoin(dag);
    };

    void accept(DAGVisitor *v) override;

    std::string get_name() override { return "Join_" + std::to_string(id); }
};

#endif  // DAG_DAGJOIN_H
