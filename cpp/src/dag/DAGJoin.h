//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGJOIN_H
#define CPP_DAGJOIN_H

#include <iostream>
#include "DAGOperator.h"

class DAGJoin : public DAGOperator {
public:
    bool stream_right = true;
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator() {
        return new DAGJoin;
    };

    void accept(DAGVisitor &v);

    std::string get_name() {
        return "Join_" + to_string(id);
    }

};


#endif //CPP_DAGJOIN_H
