#include "compiler.hpp"

#include <string>

#include <json.hpp>

#include "dag/DAG.h"
#include "optimize/optimizer.hpp"

namespace compiler {

Compiler::Compiler(const std::string &dagstr, const std::string &confstr)
    : dag_(ParseDag(dagstr)), conf_(nlohmann::json::parse(confstr).flatten()) {}

void Compiler::GenerateExecutable() {
    // Optimize
    optimize::Optimizer opt(conf_.unflatten().dump());
    opt.Run(dag_.get());
}

}  // namespace compiler
