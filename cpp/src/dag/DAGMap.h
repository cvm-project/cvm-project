//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGMAP_H
#define CPP_DAGMAP_H

#include <iostream>
#include "DAGOperator.h"

class DAGMap : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator() {
        return new DAGMap;
    };

    void accept(DAGVisitor &v);

};


#endif //CPP_DAGMAP_H
