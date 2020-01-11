#include "generate/generate_executable.hpp"

#include <cstddef>
#include <cstring>

#include <memory>

#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <boost/polymorphic_pointer_cast.hpp>
#include <nlohmann/json.hpp>

#include "dag/dag.hpp"
#include "dag/operators/compiled_pipeline.hpp"
#include "optimize/optimizer.hpp"
#include "runtime/execute_plan.hpp"
#include "utils/lib_path.hpp"

auto GenerateExecutable(const std::string &conf, const std::string &dagstr)
        -> int64_t {
    auto conf_json = nlohmann::json::parse(conf).flatten();
    conf_json.emplace("/optimizer/optimization-level", 2);

    // Parse DAG and optimize it
    std::unique_ptr<DAG> dag(ParseDag(dagstr));
    optimize::Optimizer opt(conf_json.unflatten().dump());
    opt.Run(dag.get());

    auto const compiled_pipeline_op =
            boost::polymorphic_pointer_downcast<DAGCompiledPipeline>(
                    dag->output().op);
    auto const library_path =
            get_lib_path() / "backend/gen" / compiled_pipeline_op->library_name;
    auto const function_name = compiled_pipeline_op->function_name;

    // Load generated library and register it
    auto const plan = boost::dll::import<runtime::PlanFunction>(library_path,
                                                                function_name);
    const size_t plan_id = runtime::RegisterPlan(plan);

    return plan_id;
}
