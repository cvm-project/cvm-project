#ifndef DAG_OPERATORS_MAP_CPP_HPP
#define DAG_OPERATORS_MAP_CPP_HPP

#include "operator.hpp"

class DAGMapCpp : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGMapCpp, "map_cpp");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 1; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    std::string function_name;
};

#endif  // DAG_OPERATORS_MAP_CPP_HPP
