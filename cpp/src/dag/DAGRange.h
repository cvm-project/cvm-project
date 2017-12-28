//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGRANGE_H
#define DAG_DAGRANGE_H

#include <string>

#include "DAGOperator.h"

class DAGVisitor;

class DAGRange : public DAGOperatorBase<DAGRange> {
public:
    void accept(DAGVisitor *v) override;
    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::string from;
    std::string to;
    std::string step = "1";

    constexpr static const char *kName = "range_source";
};

#endif  // DAG_DAGRANGE_H
