//
// Created by sabir on 04.07.17.
//

#ifndef CPP_FILTEROPERATOR_H
#define CPP_FILTEROPERATOR_H


#include <iostream>
#include "Operator.h"

template<class Upstream>
class FilterOperator : public Operator {
public:
    Upstream upstream;

    FilterOperator(Upstream upstream1) : upstream(upstream1) {}

    void printName() {
        std::cout << "filter op\n";
        upstream->printName();
    }
};


#endif //CPP_FILTEROPERATOR_H
