//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGOPERATOR_H
#define CPP_DAGOPERATOR_H

#include "operators/Operator.h"
#include <cstddef>
#include <iostream>
#include <vector>
#include <libs/json.hpp>
//#include <operators/Operator.h>

class DAGOperator {
public:
    std::vector<DAGOperator *> predecessors;
    std::vector<DAGOperator *> successors;
    std::string llvm_ir;
    std::string output_type;

    virtual void initWithJson(nlohmann::basic_json<>) {};

};

#endif //CPP_DAGOPERATOR_H
