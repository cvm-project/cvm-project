//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCE_H
#define DAG_DAGREDUCE_H

#include "DAGOperator.h"

class DAGReduce : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator() { return new DAGReduce; };

    void accept(DAGVisitor *v);

    std::string get_name() { return "Reduce_" + std::to_string(id); }
};

#endif  // DAG_DAGREDUCE_H
