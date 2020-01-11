#ifndef DAG_OPERATORS_ROW_SCAN_HPP
#define DAG_OPERATORS_ROW_SCAN_HPP

#include <nlohmann/json.hpp>

#include "operator.hpp"

class DAGRowScan : public DAGOperator {
    JITQ_DAGOPERATOR(DAGRowScan, "row_scan");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    bool add_index = false;
};

#endif  // DAG_OPERATORS_ROW_SCAN_HPP
