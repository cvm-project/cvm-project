//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCARTESIAN_H
#define DAG_DAGCARTESIAN_H

#include "DAGOperator.h"

class DAGCartesian : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;
    bool stream_right = true;

    static DAGOperator *make_dag_operator() { return new DAGCartesian; };

    void accept(DAGVisitor *v) override;

    std::string get_name() override {
        return "Cartesian_" + std::to_string(id);
    }
};

#endif  // DAG_DAGCARTESIAN_H
