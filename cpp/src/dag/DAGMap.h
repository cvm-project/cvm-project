//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGMAP_H
#define CPP_DAGMAP_H

#include <iostream>
#include <operators/MapOperator.h>
#include "DAGOperator.h"

class DAGMap : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator() {
        return new DAGMap;
    };
};


#endif //CPP_DAGMAP_H
