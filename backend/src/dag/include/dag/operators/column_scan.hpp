#ifndef DAG_OPERATORS_COLUMN_SCAN_HPP
#define DAG_OPERATORS_COLUMN_SCAN_HPP

#include <nlohmann/json.hpp>

#include "operator.hpp"

class DAGColumnScan : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGColumnScan, "column_scan");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    bool add_index = false;
};

#endif  // DAG_OPERATORS_COLUMN_SCAN_HPP
