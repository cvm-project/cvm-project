//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCOLLECTION_H
#define DAG_DAGCOLLECTION_H

#include "DAGOperator.h"

class DAGCollection : public DAGOperator {
public:
    explicit DAGCollection(DAG *const dag) : DAGOperator(dag) {}

    bool add_index = false;
    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGCollection(dag);
    };

    std::string get_name() override {
        return "Collection_" + std::to_string(id);
    }

    void accept(DAGVisitor *v) override;
    void parse_json(const nlohmann::json &json) override;
};

#endif  // DAG_DAGCOLLECTION_H
