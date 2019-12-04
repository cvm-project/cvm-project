#ifndef DAG_OPERATORS_REDUCE_BY_INDEX_HPP
#define DAG_OPERATORS_REDUCE_BY_INDEX_HPP

#include "operator.hpp"

class DAGReduceByIndex : public DAGOperator {
    JITQ_DAGOPERATOR(DAGReduceByIndex, "reduce_by_index");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    int64_t min = 0;
    int64_t max = 0;
};

#endif  // DAG_OPERATORS_REDUCE_BY_INDEX_HPP
