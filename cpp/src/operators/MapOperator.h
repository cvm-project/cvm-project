//
// Created by sabir on 04.07.17.
//

#ifndef CPP_MAPOPERATOR_H
#define CPP_MAPOPERATOR_H


#include <iostream>
#include "Operator.h"

template <class Upstream>
class MapOperator : public Operator {
public:
    Upstream upstream;

    MapOperator(Upstream upstream1) : upstream(upstream1) {}

    void printName() {
        std::cout << "map op\n";
        upstream->printName();
    }
};


#endif //CPP_MAPOPERATOR_H
