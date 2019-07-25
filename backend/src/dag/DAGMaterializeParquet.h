#ifndef DAG_DAGMATERIALIZEPARQUET_H
#define DAG_DAGMATERIALIZEPARQUET_H

#include "DAGOperator.h"

class DAGMaterializeParquet : public DAGOperator {
    JITQ_DAGOPERATOR(DAGMaterializeParquet, "materialize_parquet_file");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::vector<std::string> column_names;
};

#endif  // DAG_DAGMATERIALIZEPARQUET_H
