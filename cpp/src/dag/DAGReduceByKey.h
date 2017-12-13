//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCEBYKEY_H
#define DAG_DAGREDUCEBYKEY_H

#include "DAGOperator.h"
class DAGVisitor;

class DAGReduceByKey : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() { return new DAGReduceByKey; };

    void accept(DAGVisitor *v) override;

    std::string get_name() override {
        return "ReduceByKey_" + std::to_string(id);
    }
};

#endif  // DAG_DAGREDUCEBYKEY_H
