//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGREDUCEBYKEY_H
#define CPP_DAGREDUCEBYKEY_H

#include "DAGOperator.h"

class DAGReduceByKey : public DAGOperator {
public:

    static DAGOperator *make_dag_operator() {
        return new DAGReduceByKey;
    };

    void accept(DAGVisitor &v);

};


#endif //CPP_DAGREDUCEBYKEY_H
