//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGCOLLECTION_H
#define CPP_DAGCOLLECTION_H

#include "operators/Operator.h"
#include "operators/CollectionSourceOperator.h"
#include "DAGOperator.h"

class DAGCollection : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() {
        return new DAGCollection;
    };

    void initWithJson(nlohmann::basic_json<>);

private:
    nlohmann::basic_json<> values;
};


#endif //CPP_DAGCOLLECTION_H
