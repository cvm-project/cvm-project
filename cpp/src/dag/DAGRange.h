//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGRANGE_H
#define CPP_DAGRANGE_H

#include "DAGOperator.h"

class DAGRange : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() {
        return new DAGRange;
    };

    void accept(DAGVisitor &v);

private:
    int start;
    int end;
    int step = 1;
};


#endif //CPP_DAGRANGE_H
