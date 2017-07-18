//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGJOIN_H
#define CPP_DAGJOIN_H

#include <iostream>
#include "DAGOperator.h"
#include "../operators/JoinOperator.h"

class DAGJoin : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator() {
        return new DAGJoin;
    };


    Operator *make_operator() {
//        if (predecessors.size() < 2) {
//            throw std::logic_error("Join predecessors have to be set before instantiating operator template");
//        }
//        return this->make_join(predecessors[0]->make_operator(), predecessors[1]->make_operator());
    };

    template<class Tuple, class Upstream1, class Upstream2>
    JoinOperator<Upstream1, Upstream2, Tuple> *make_join(Upstream1 *upstream1, Upstream2 *upstream2) {
        return new JoinOperator<Upstream1, Upstream2, Tuple>(upstream1, upstream2);
    };

};


#endif //CPP_DAGJOIN_H
