#ifndef DAG_OPERATORS_MATERIALIZE_PARQUET_HPP
#define DAG_OPERATORS_MATERIALIZE_PARQUET_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "operator.hpp"

class DAGMaterializeParquet : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMaterializeParquet, "materialize_parquet_file");

public:
    auto num_in_ports() const -> size_t override { return 2; }
    auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::vector<std::string> column_names;
};

#endif  // DAG_OPERATORS_MATERIALIZE_PARQUET_HPP
