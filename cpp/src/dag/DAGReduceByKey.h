//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCEBYKEY_H
#define DAG_DAGREDUCEBYKEY_H

#include "DAGOperator.h"
class DAGVisitor;

class DAGReduceByKey : public DAGOperator {
public:
    explicit DAGReduceByKey(DAG *const dag) : DAGOperator(dag) {}

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGReduceByKey(dag);
    };

    void accept(DAGVisitor *v) override;

    std::string get_name() override {
        return "ReduceByKey_" + std::to_string(id);
    }
};

#endif  // DAG_DAGREDUCEBYKEY_H
