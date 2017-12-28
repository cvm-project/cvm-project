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

    constexpr static const char *kName = "reduce_by_key";
    std::string name() const override { return kName; }
};

#endif  // DAG_DAGREDUCEBYKEY_H
