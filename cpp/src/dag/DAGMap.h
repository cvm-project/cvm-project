//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGMAP_H
#define CPP_DAGMAP_H

#include <iostream>
#include <operators/MapOperator.h>
#include "DAGOperator.h"

class DAGMap : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator() {
        return new DAGMap;
    };


    Operator *make_operator() {
        if (!predecessors.size()) {
            throw std::logic_error("Map predecessor has to be set before instantiating operator template");
        }
        return this->make_map(predecessors[0]->make_operator());
    };

    template<class Upstream>
    MapOperator<Upstream> *make_map(Upstream _) {
        return new MapOperator<Upstream>(_);
    };

};


#endif //CPP_DAGMAP_H
