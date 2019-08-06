#ifndef DAG_OPERATORS_PARQUET_SCAN_HPP
#define DAG_OPERATORS_PARQUET_SCAN_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "operator.hpp"

class DAGParquetScan : public DAGOperator {
    JITQ_DAGOPERATOR(DAGParquetScan, "parquet_scan");

public:
    using RangeFilters = std::vector<std::pair<std::string, std::string>>;
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::vector<uint16_t> column_indexes;
    std::vector<RangeFilters> column_range_filters;
    std::string filesystem;
};

#endif  // DAG_OPERATORS_PARQUET_SCAN_HPP
