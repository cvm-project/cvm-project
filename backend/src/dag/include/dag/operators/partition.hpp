#ifndef DAG_OPERATORS_PARTITION_HPP
#define DAG_OPERATORS_PARTITION_HPP

#include <string>

#include "operator.hpp"

class DAGPartition : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGPartition, "partition");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 2; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    size_t seed = 0;
};

#endif  // DAG_OPERATORS_PARTITION_HPP
