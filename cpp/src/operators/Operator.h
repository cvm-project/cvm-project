//
// Created by sabir on 01.07.17.
//

#ifndef CPP_OPERATOR_H
#define CPP_OPERATOR_H

#include <stdlib.h>  //size_t def

#include "libs/optional.h"

#define INLINE __attribute__((always_inline))

class Operator {
public:

    virtual void open()=0;

    virtual void close()=0;
};

#endif //CPP_OPERATOR_H