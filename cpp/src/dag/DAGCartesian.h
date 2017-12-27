//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCARTESIAN_H
#define DAG_DAGCARTESIAN_H

#include "DAGOperator.h"

class DAGCartesian : public DAGOperator {
public:
    explicit DAGCartesian(DAG *const dag) : DAGOperator(dag) {}

    bool stream_right = true;

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGCartesian(dag);
    };

    void accept(DAGVisitor *v) override;

    constexpr static const char *kName = "cartesian";
    std::string name() const override { return kName; }
};

#endif  // DAG_DAGCARTESIAN_H
