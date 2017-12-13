//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCARTESIAN_H
#define DAG_DAGCARTESIAN_H

#include "DAGOperator.h"

class DAGCartesian : public DAGOperator {
public:
    explicit DAGCartesian(DAG *const dag) : DAGOperator(dag) {}

    static const std::string DAG_OP_NAME;
    bool stream_right = true;

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGCartesian(dag);
    };

    void accept(DAGVisitor *v) override;

    std::string get_name() override {
        return "Cartesian_" + std::to_string(id);
    }
};

#endif  // DAG_DAGCARTESIAN_H
