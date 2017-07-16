//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGOPERATOR_H
#define CPP_DAGOPERATOR_H

#include "operators/Operator.h"
#include <cstddef>
#include <iostream>
#include <vector>
//#include <operators/Operator.h>

class DAGOperator {
public:
    std::vector<DAGOperator *> predecessors;
    std::vector<DAGOperator *> successors;
    std::string llvm_ir;

    virtual Operator *make_operator() = 0;

};

#endif //CPP_DAGOPERATOR_H
