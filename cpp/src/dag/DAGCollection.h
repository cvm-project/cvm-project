//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGCOLLECTION_H
#define CPP_DAGCOLLECTION_H

#include "DAGOperator.h"

class DAGCollection : public DAGOperator {
public:
    bool add_index = false;
    static DAGOperator *make_dag_operator() { return new DAGCollection; };

    std::string get_name() { return "Collection_" + to_string(id); }

    void accept(DAGVisitor &v);
    void parse_json(nlohmann::json);
};

#endif  // CPP_DAGCOLLECTION_H
