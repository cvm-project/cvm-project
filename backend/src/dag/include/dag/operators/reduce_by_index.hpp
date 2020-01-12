#ifndef DAG_OPERATORS_REDUCE_BY_INDEX_HPP
#define DAG_OPERATORS_REDUCE_BY_INDEX_HPP

#include "operator.hpp"

class DAGReduceByIndex : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGReduceByIndex, "reduce_by_index");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    int64_t min = 0;
    int64_t max = 0;
};

#endif  // DAG_OPERATORS_REDUCE_BY_INDEX_HPP
