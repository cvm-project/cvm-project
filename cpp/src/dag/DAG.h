//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAG_H
#define CPP_DAG_H


#include "DAGOperator.h"
#include "iostream"

class DAG {
public:
    DAGOperator *sink;
    std::string action;
};


#endif //CPP_DAG_H
