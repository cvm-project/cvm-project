#include "compiler.hpp"

#include <string>
#include <unordered_map>

#include <json.hpp>

#include "code_gen/common/BackEnd.hpp"
#include "code_gen/cpp/BackEnd.hpp"
#include "dag/DAG.h"
#include "dag/dag_factory.hpp"
#include "optimize/Optimizer.h"

namespace compiler {

Compiler::Compiler(const std::string &dagstr, const std::string &confstr)
    : dag_(DagFactory::instance().ParseDag(dagstr)),
      conf_(nlohmann::json::parse(confstr).flatten()) {}

void Compiler::GenerateExecutable() {
    // Optimize
    Optimizer opt;
    if (conf_.value("/optimizer/optimization-level", 0) > 0) {
        opt.run(dag_.get());
    }

    // Select code gen back-end
    // TODO(ingo): pass back-end-specific config to back-end
    using CodeGenBackEnd = std::unique_ptr<code_gen::common::BackEnd>;
    std::unordered_map<std::string, CodeGenBackEnd> code_generators;
    code_generators.emplace("cpp",
                            CodeGenBackEnd(new code_gen::cpp::BackEnd()));

    const std::string codegen_backend = conf_.value("/codegen/backend", "cpp");
    const auto code_gen_backend = code_generators.at(codegen_backend).get();

    if (conf_.value("/optimizer/codegen", true)) {
        // Generate code
        code_gen_backend->GenerateCode(dag_.get());

        // Compile to native machine code
        // TODO(ingo): think again about this interface
        code_gen_backend->Compile(conf_.value("/backend/counter", 0));
    }
}

}  // namespace compiler
