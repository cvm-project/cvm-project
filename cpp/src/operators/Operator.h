//
// Created by sabir on 01.07.17.
//

#ifndef CPP_OPERATOR_H
#define CPP_OPERATOR_H
#include <libs/optional.h>

class Operator {
public:
    virtual void printName()=0;
    virtual void open()=0;
};

#endif //CPP_OPERATOR_H