//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGMAP_H
#define DAG_DAGMAP_H

#include <iostream>

#include "DAGOperator.h"

class DAGVisitor;
class DAGMap : public DAGOperator {
public:
    explicit DAGMap(DAG *const dag) : DAGOperator(dag) {}

    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGMap(dag);
    };

    void accept(DAGVisitor *v) override;

    std::string get_name() override { return "Map_" + std::to_string(id); }
};

#endif  // DAG_DAGMAP_H
