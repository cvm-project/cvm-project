#include "generate/generate_executable.hpp"

#include <memory>

#include <nlohmann/json.hpp>

#include "dag/dag.hpp"
#include "optimize/optimizer.hpp"
#include "runtime/execute_plan.hpp"

auto GenerateExecutable(const std::string &conf, const std::string &dagstr)
        -> int64_t {
    auto conf_json = nlohmann::json::parse(conf)["optimizer"];
    conf_json["optimization-level"] = 2;

    // Parse DAG and optimize it
    std::unique_ptr<DAG> dag(ParseDag(dagstr));
    optimize::Optimizer opt(conf_json.dump());
    opt.Run(dag.get());

    // Register as plan and return ID
    return runtime::RegisterPlan(std::move(dag));
}
