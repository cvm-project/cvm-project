//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGROWSCAN_H
#define DAG_DAGROWSCAN_H

#include <nlohmann/json.hpp>

#include "DAGOperator.h"

class DAGRowScan : public DAGOperator {
    JITQ_DAGOPERATOR(DAGRowScan, "row_scan");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    bool add_index = false;
};

#endif  // DAG_DAGROWSCAN_H
