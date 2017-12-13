//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGFILTER_H
#define DAG_DAGFILTER_H

#include "DAGOperator.h"

class DAGVisitor;

class DAGFilter : public DAGOperator {
public:
    explicit DAGFilter(DAG *const dag) : DAGOperator(dag) {}

    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGFilter(dag);
    };

    void accept(DAGVisitor *v) override;

    std::string get_name() override { return "Filter_" + std::to_string(id); }

    DAGFilter *copy();
};

#endif  // DAG_DAGFILTER_H
