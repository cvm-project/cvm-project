#include "optimize/code_gen.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "code_gen/code_gen.hpp"
#include "dag/dag.hpp"
#include "dag/operators/compiled_pipeline.hpp"
#include "optimize/optimizer.hpp"

namespace optimize {

void CodeGen::Run(DAG *const dag, const std::string &config) const {
    // Generate code and compile
    auto const [lib_path, func_name] = code_gen::cpp::GenerateCode(dag, config);

    // Create new DAGCompiledPipeline operator
    auto pipeline_op_ptr = std::make_unique<DAGCompiledPipeline>();
    auto const pipeline_op = pipeline_op_ptr.get();

    pipeline_op->tuple = dag->output().op->tuple;
    pipeline_op->library_name = lib_path;
    pipeline_op->function_name = func_name;
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

    for (size_t i = 0; i < inputs.size(); i++) {
        dag->add_input(inputs[i].first, pipeline_op, i);
    }

    for (size_t i = 0; i < outputs.size(); i++) {
        dag->set_output(outputs[i].first, pipeline_op, i);
    }
}

}  // namespace optimize
