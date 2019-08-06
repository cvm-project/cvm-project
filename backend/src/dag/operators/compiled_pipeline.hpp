#ifndef DAG_OPERATORS_COMPILED_PIPELINE_HPP
#define DAG_OPERATORS_COMPILED_PIPELINE_HPP

#include <string>

#include "dag/dag.hpp"
#include "operator.hpp"

class DAGCompiledPipeline : public DAGOperator {
    JITQ_DAGOPERATOR(DAGCompiledPipeline, "compiled_pipeline");

public:
    size_t num_in_ports() const override { return num_inputs; }
    size_t num_out_ports() const override { return 1; }

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;

    size_t num_inputs{};
    std::string library_name;
    std::string function_name;
};

#endif  // DAG_OPERATORS_COMPILED_PIPELINE_HPP
