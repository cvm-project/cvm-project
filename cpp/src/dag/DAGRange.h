//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGRANGE_H
#define DAG_DAGRANGE_H

#include <string>

#include "DAGOperator.h"

class DAGVisitor;

class DAGRange : public DAGOperator {
public:
    explicit DAGRange(DAG *const dag) : DAGOperator(dag) {}

    static DAGOperator *make_dag_operator(DAG *const dag) {
        return new DAGRange(dag);
    };

    void accept(DAGVisitor *v) override;

    void parse_json(const nlohmann::json &json) override;

    std::string from;
    std::string to;
    std::string step = "1";

    constexpr static const char *kName = "range_source";
    std::string name() const override { return kName; }
};

#endif  // DAG_DAGRANGE_H
