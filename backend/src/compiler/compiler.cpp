#include "compiler.hpp"

#include <string>
#include <unordered_map>

#include <json.hpp>

#include "code_gen/common/back_end.hpp"
#include "code_gen/cpp/back_end.hpp"
#include "dag/DAG.h"
#include "optimize/optimizer.hpp"

namespace compiler {

Compiler::Compiler(const std::string &dagstr, const std::string &confstr)
    : dag_(ParseDag(dagstr)), conf_(nlohmann::json::parse(confstr).flatten()) {}

void Compiler::GenerateExecutable() {
    // Optimize
    optimize::Optimizer opt(conf_.unflatten().dump());
    opt.Run(dag_.get());

    // Select code gen back-end
    // TODO(ingo): pass back-end-specific config to back-end
    using CodeGenBackEnd = std::unique_ptr<code_gen::common::BackEnd>;
    std::unordered_map<std::string, CodeGenBackEnd> code_generators;
    code_generators.emplace("cpp", CodeGenBackEnd(new code_gen::cpp::BackEnd(
                                           conf_.unflatten().dump())));

    const std::string codegen_backend = conf_.value("/codegen/backend", "cpp");
    const auto code_gen_backend = code_generators.at(codegen_backend).get();

    if (conf_.value("/optimizer/codegen", true)) {
        // Generate code and compile
        code_gen_backend->Run(dag_.get());
    }
}

}  // namespace compiler
