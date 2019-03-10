#include "code_gen.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#include <json.hpp>

#include "code_gen/common/back_end.hpp"
#include "code_gen/cpp/back_end.hpp"
#include "dag/DAG.h"
#include "dag/DAGCompiledPipeline.h"
#include "optimize/optimizer.hpp"

namespace optimize {

void CodeGen::Run(DAG *const dag, const std::string &config) const {
    const nlohmann::json conf = nlohmann::json::parse(config).flatten();

    // Select code gen back-end
    // TODO(ingo): pass back-end-specific config to back-end
    using CodeGenBackEnd = std::unique_ptr<code_gen::common::BackEnd>;
    std::unordered_map<std::string, CodeGenBackEnd> code_generators;
    code_generators.emplace("cpp", CodeGenBackEnd(new code_gen::cpp::BackEnd(
                                           conf.unflatten().dump())));

    const std::string codegen_backend = conf.value("/backend", "cpp");
    const auto code_gen_backend = code_generators.at(codegen_backend).get();

    // Generate code and compile
    auto const lib_path = code_gen_backend->Run(dag);

    // Create new DAGCompiledPipeline operator
    std::unique_ptr<DAGCompiledPipeline> pipeline_op_ptr(
            new DAGCompiledPipeline());
    auto const pipeline_op = pipeline_op_ptr.get();

    pipeline_op->tuple = dag->output().op->tuple;
    pipeline_op->library_name = lib_path;
    pipeline_op->function_name = "execute";
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
