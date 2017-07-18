//
// Created by sabir on 04.07.17.
//

#ifndef CPP_JOINOPERATOR_H
#define CPP_JOINOPERATOR_H

#include <iostream>
#include "Operator.h"

template<class Upstream1, class Upstream2, class Tuple>
class JoinOperator : public Operator {
public:
    Upstream1 *upstream1;
    Upstream2 *upstream2;

    JoinOperator(Upstream1 upstream1, Upstream2 upstream2) : upstream1(upstream1), upstream2(upstream2) {};

    void printName() {
        std::cout << "join op\n";
        upstream1->printName();
        upstream2->printName();
    }

    Tuple next(){}
};


#endif //CPP_JOINOPERATOR_H
