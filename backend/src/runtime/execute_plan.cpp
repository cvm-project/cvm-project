#include "execute_plan.hpp"

#include <cstring>
#include <vector>

#include "values/json_parsing.hpp"

namespace runtime {
namespace impl {

using PlanRegistry = std::vector<Plan>;

static PlanRegistry *plan_registry() {
    static PlanRegistry registry;
    return &registry;
}

}  // namespace impl

std::string ExecutePlan(const size_t plan_id, const std::string &inputs_str) {
    const auto inputs =
            runtime::values::ConvertFromJsonString(inputs_str.c_str());
    const auto plan = impl::plan_registry()->at(plan_id);
    const auto ret = plan(inputs);
    return runtime::values::ConvertToJsonString(ret);
}

size_t RegisterPlan(const Plan &plan) {
    impl::plan_registry()->emplace_back(plan);
    return impl::plan_registry()->size() - 1;
}

}  // namespace runtime
