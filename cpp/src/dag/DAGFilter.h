//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGFILTER_H
#define CPP_DAGFILTER_H


#include <operators/Operator.h>
#include <stdexcept>
#include <operators/FilterOperator.h>
#include "DAGOperator.h"
#include "DAGRange.h"

class DAGFilter : public DAGOperator {

public:
    static const std::string DAG_OP_NAME;


    static DAGOperator *make_dag_operator() {
        return new DAGFilter;
    };

    Operator *make_operator() {
        if (!predecessors.size()) {
            throw std::logic_error("Filter predecessor has to be set before instantiating operator template");
        }
        return this->make_filter(predecessors[0]->make_operator());
    };

    template<class Upstream>
    FilterOperator<Upstream> *make_filter(Upstream _) {
        return new FilterOperator<Upstream>(_);
    };
};



#endif //CPP_DAGFILTER_H
