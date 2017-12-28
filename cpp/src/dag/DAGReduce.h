//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCE_H
#define DAG_DAGREDUCE_H

#include "DAGOperator.h"

class DAGReduce : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;

    explicit DAGReduce(DAG *const dag) : DAGOperator(dag) {}

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGReduce(dag);
    };

    void accept(DAGVisitor *v) override;

    constexpr static const char *kName = "reduce";
    std::string name() const override { return kName; }
};

#endif  // DAG_DAGREDUCE_H
