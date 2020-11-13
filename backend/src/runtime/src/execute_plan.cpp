#include "runtime/execute_plan.hpp"

#include <cstring>

#include <functional>
#include <optional>
#include <vector>

#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <boost/polymorphic_pointer_cast.hpp>
#include <nlohmann/json.hpp>

#include "dag/dag.hpp"
#include "dag/operators/compiled_pipeline.hpp"
#include "runtime/jit/values/json_parsing.hpp"
#include "runtime/memory/values.hpp"
#include "utils/lib_path.hpp"
#include "utils/registry.hpp"

namespace runtime {

using PlanFunction =
        runtime::values::VectorOfValues(runtime::values::VectorOfValues);
using PlanFunctor = std::function<PlanFunction>;

struct Plan {
    std::unique_ptr<const DAG> dag;
    std::optional<PlanFunctor> functor;
};

using PlanRegistry = utils::Registry<Plan, size_t>;

auto RegisterPlan(std::unique_ptr<const DAG> dag) -> size_t {
    static size_t next_plan_id = 0;
    auto const plan_id = next_plan_id++;
    PlanRegistry::Register(plan_id,
                           std::make_unique<Plan>(Plan{std::move(dag), {}}));
    return plan_id;
}

auto DumpDag(const size_t plan_id) -> std::string {
    auto* const plan = PlanRegistry::at(plan_id);
    const nlohmann::json dag_json(plan->dag);
    return dag_json.dump();
}

auto LoadPlan(const DAG* const dag) -> PlanFunctor {
    auto* const compiled_pipeline_op =
            boost::polymorphic_pointer_downcast<DAGCompiledPipeline>(
                    dag->output().op);

    auto const library_path =
            get_lib_path() / "backend/gen" / compiled_pipeline_op->library_name;
    auto const function_name = compiled_pipeline_op->function_name;

    return boost::dll::import<runtime::PlanFunction>(library_path,
                                                     function_name);
}

void LoadPlan(const size_t plan_id) {
    auto* const plan = PlanRegistry::at(plan_id);
    plan->functor = LoadPlan(plan->dag.get());
}

auto ExecutePlan(const PlanFunctor& functor, const std::string& inputs_str)
        -> std::string {
    const auto inputs =
            runtime::values::ConvertFromJsonString(inputs_str.c_str());
    const auto ret = functor(inputs);
    runtime::memory::Increment(ret);
    return runtime::values::ConvertToJsonString(ret);
}

auto ExecutePlan(const DAG* const dag, const std::string& inputs_str)
        -> std::string {
    auto const functor = LoadPlan(dag);
    return ExecutePlan(functor, inputs_str);
}

auto ExecutePlan(const size_t plan_id, const std::string& inputs_str)
        -> std::string {
    auto* plan = PlanRegistry::at(plan_id);
    if (!plan->functor) {
        plan->functor = LoadPlan(plan->dag.get());
    }
    return ExecutePlan(plan->functor.value(), inputs_str);
}

}  // namespace runtime
