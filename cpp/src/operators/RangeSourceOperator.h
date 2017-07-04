//
// Created by sabir on 04.07.17.
//

#ifndef CPP_RANGESOURCEOPERATOR_H
#define CPP_RANGESOURCEOPERATOR_H


#include <iostream>
#include "Operator.h"

class RangeSourceOperator : public Operator {
public:
    void printName() {
        std::cout << "range op\n";
    }

//    Tuple next(){return NULL;}
};


#endif //CPP_RANGESOURCEOPERATOR_H
