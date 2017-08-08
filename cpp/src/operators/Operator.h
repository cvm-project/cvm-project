//
// Created by sabir on 01.07.17.
//

#ifndef CPP_OPERATOR_H
#define CPP_OPERATOR_H

#include <libs/optional.h>
#include "utils/debug_print.h"
#include "utils/tuple_to_string.h"


#define INLINE __attribute__((always_inline))

class Operator {
public:
    virtual void printName()=0;

    virtual void open()=0;

    virtual void close()=0;
};

#endif //CPP_OPERATOR_H