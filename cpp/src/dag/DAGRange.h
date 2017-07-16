//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGRANGE_H
#define CPP_DAGRANGE_H

#include "operators/Operator.h"
#include "operators/RangeSourceOperator.h"
#include "DAGOperator.h"

class DAGRange : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() {
        return new DAGRange;
    };

    Operator *make_operator() {
        return new RangeSourceOperator;
    }

};


#endif //CPP_DAGRANGE_H
