//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGREDUCEBYKEY_H
#define CPP_DAGREDUCEBYKEY_H

#include "DAGOperator.h"
class DAGVisitor;

class DAGReduceByKey : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() { return new DAGReduceByKey; };

    void accept(DAGVisitor *v);

    std::string get_name() { return "ReduceByKey_" + std::to_string(id); }
};

#endif  // CPP_DAGREDUCEBYKEY_H
