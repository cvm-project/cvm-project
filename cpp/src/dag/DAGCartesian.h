//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGCARTESIAN_H
#define CPP_DAGCARTESIAN_H

#include "DAGOperator.h"

class DAGCartesian : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;
    bool stream_right = true;

    static DAGOperator *make_dag_operator() {
        return new DAGCartesian;
    };

    void accept(DAGVisitor &v);

    std::string get_name() {
        return "Cartesian_" + to_string(id);
    }

};


#endif //CPP_DAGCARTESIAN_H
