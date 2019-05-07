#include "code_gen.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "code_gen/cpp/code_gen.hpp"
#include "dag/DAG.h"
#include "dag/DAGCompiledPipeline.h"
#include "optimize/optimizer.hpp"

namespace optimize {

void CodeGen::Run(DAG *const dag, const std::string &config) const {
    const nlohmann::json conf = nlohmann::json::parse(config).flatten();

    // Generate code and compile
    auto const lib_path = code_gen::cpp::GenerateCode(dag, config);

    // Create new DAGCompiledPipeline operator
    std::unique_ptr<DAGCompiledPipeline> pipeline_op_ptr(
            new DAGCompiledPipeline());
    auto const pipeline_op = pipeline_op_ptr.get();

    pipeline_op->tuple = dag->output().op->tuple;
    pipeline_op->library_name = lib_path;
    pipeline_op->function_name = "execute_pipelines";
    pipeline_op->num_inputs = 0;

    // Remember inputs and outputs
    std::vector<std::pair<int, DAG::FlowTip>> inputs;
    for (auto const &input : dag->inputs()) {
        inputs.emplace_back(input);
    }

    std::vector<std::pair<int, DAG::FlowTip>> outputs;
    for (auto const &output : dag->outputs()) {
        outputs.emplace_back(output);
    }

    // Clear DAG
    dag->Clear();

    // Add new operator and set inputs and outputs
    dag->AddOperator(pipeline_op_ptr.release());

    size_t next_input_port = 0;
    for (auto const &input : inputs) {
        dag->add_input(input.first, pipeline_op, next_input_port++);
    }

    size_t next_output_port = 0;
    for (auto const &output : outputs) {
        dag->set_output(output.first, pipeline_op, next_output_port++);
    }
}

}  // namespace optimize
