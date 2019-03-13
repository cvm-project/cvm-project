#ifndef DAG_DAGPROJECTION_H
#define DAG_DAGPROJECTION_H

#include <vector>

#include <json.hpp>

#include "DAGOperator.h"

class DAGProjection : public DAGOperatorBase<DAGProjection> {
public:
    constexpr static const char *kName = "projection";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::vector<size_t> positions;
};

#endif  // DAG_DAGPROJECTION_H
