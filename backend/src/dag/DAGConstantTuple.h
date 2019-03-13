#ifndef DAG_DAGCONSTANTTUPLE_H
#define DAG_DAGCONSTANTTUPLE_H

#include <string>
#include <vector>

#include <json.hpp>

#include "DAGOperator.h"

class DAGConstantTuple : public DAGOperator {
    JITQ_DAGOPERATOR(DAGConstantTuple, "constant_tuple");

public:
    size_t num_in_ports() const override { return 0; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::vector<std::string> values;
};

#endif  // DAG_DAGCONSTANTTUPLE_H
