#ifndef DAG_DAGCOLUMNSCAN_H
#define DAG_DAGCOLUMNSCAN_H

#include <nlohmann/json.hpp>

#include "DAGOperator.h"

class DAGColumnScan : public DAGOperator {
    JITQ_DAGOPERATOR(DAGColumnScan, "column_scan");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    bool add_index = false;
};

#endif  // DAG_DAGCOLUMNSCAN_H
