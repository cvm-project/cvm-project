#ifndef DAG_DAGCONSTANTTUPLE_H
#define DAG_DAGCONSTANTTUPLE_H

#include <string>
#include <vector>

#include <json.hpp>

#include "DAGOperator.h"

class DAGConstantTuple : public DAGOperatorBase<DAGConstantTuple> {
public:
    constexpr static const char *kName = "constant_tuple";
    constexpr static size_t kNumInPorts = 0;
    constexpr static size_t kNumOutPorts = 1;

    std::vector<std::string> values;

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;
};

#endif  // DAG_DAGCONSTANTTUPLE_H
