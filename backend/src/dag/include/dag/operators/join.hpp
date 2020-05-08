#ifndef DAG_OPERATORS_JOIN_HPP
#define DAG_OPERATORS_JOIN_HPP

#include "operator.hpp"

class DAGJoin : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGJoin, "join");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 2; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    int num_keys = 1;
};

#endif  // DAG_OPERATORS_JOIN_HPP
