#ifndef DAG_DAGPARAMETERLOOKUP_H
#define DAG_DAGPARAMETERLOOKUP_H

#include <string>
#include <vector>

#include <json.hpp>

#include "DAGOperator.h"

class DAGParameterLookup : public DAGOperatorBase<DAGParameterLookup> {
public:
    constexpr static const char *kName = "parameter_lookup";
    constexpr static size_t kNumInPorts = 0;
    constexpr static size_t kNumOutPorts = 1;

    size_t parameter_num{};

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;
};

#endif  // DAG_DAGPARAMETERLOOKUP_H
