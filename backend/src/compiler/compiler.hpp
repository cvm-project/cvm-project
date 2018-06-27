#ifndef COMPILER_COMPILER_HPP
#define COMPILER_COMPILER_HPP

#include <memory>

#include <json.hpp>

#include "dag/DAG.h"

namespace compiler {

class Compiler {
public:
    explicit Compiler(const std::string &dagstr,
                      const std::string &confstr = "{}");

    const DAG *dag() const { return dag_.get(); }
    std::string conf() const { return conf_.unflatten().dump(); }

    void GenerateExecutable();

private:
    std::unique_ptr<DAG> dag_;
    nlohmann::json conf_;
};

}  // namespace compiler

#endif  // COMPILER_COMPILER_HPP
