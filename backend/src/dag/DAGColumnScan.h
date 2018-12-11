#ifndef DAG_DAGCOLUMNSCAN_H
#define DAG_DAGCOLUMNSCAN_H

#include "DAGOperator.h"

class DAGColumnScan : public DAGOperatorBase<DAGColumnScan> {
public:
    bool add_index = false;

    constexpr static const char *kName = "column_scan";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;
};

#endif  // DAG_DAGCOLUMNSCAN_H
