#include "generate_executable.hpp"

#include <cstddef>
#include <cstring>
#include <memory>

#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <boost/polymorphic_pointer_cast.hpp>

#include <json.hpp>

#include "dag/DAG.h"
#include "dag/DAGCompiledPipeline.h"
#include "optimize/optimizer.hpp"
#include "runtime/execute_plan.hpp"
#include "runtime/free.hpp"

int GenerateExecutable(const char *const conf, const char *const dagstr) {
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

void FreeResult(const char *const s) {
    runtime::FreeValues(s);
    free(const_cast<void *>(reinterpret_cast<const void *>(s)));
}

const char *ExecutePlan(int plan_id, const char *input_str) {
    const auto ret_str = runtime::ExecutePlan(plan_id, input_str);
    const auto ret_ptr = reinterpret_cast<char *>(malloc(ret_str.size() + 1));
    strncpy(ret_ptr, ret_str.c_str(), ret_str.size() + 1);
    return ret_ptr;
}

void ClearPlans() { runtime::ClearPlans(); }
