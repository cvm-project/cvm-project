#include "runtime/execute_plan.hpp"

#include <cstring>
#include <vector>

#include "runtime/jit/values/json_parsing.hpp"
#include "runtime/memory/values.hpp"
#include "utils/registry.hpp"

namespace runtime {

using PlanRegistry = utils::Registry<Plan, size_t>;

std::string ExecutePlan(const size_t plan_id, const std::string &inputs_str) {
    const auto inputs = values::ConvertFromJsonString(inputs_str.c_str());
    const auto plan = *(PlanRegistry::at(plan_id));
    const auto ret = plan(inputs);
    memory::Increment(ret);
    return values::ConvertToJsonString(ret);
}

size_t RegisterPlan(const Plan &plan) {
    auto const plan_id = PlanRegistry::num_objects();
    PlanRegistry::Register(plan_id, std::make_unique<Plan>(plan));
    return plan_id;
}

}  // namespace runtime
