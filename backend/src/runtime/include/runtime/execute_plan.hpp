#ifndef RUNTIME_EXECUTE_PLAN_HPP
#define RUNTIME_EXECUTE_PLAN_HPP

#include <functional>
#include <string>

#include "runtime/jit/values/json_parsing.hpp"

namespace runtime {

using PlanFunction =
        runtime::values::VectorOfValues(runtime::values::VectorOfValues);
using Plan = std::function<PlanFunction>;

std::string ExecutePlan(size_t plan_id, const std::string& inputs_str);
size_t RegisterPlan(const Plan& plan);

}  // namespace runtime

#endif  // RUNTIME_EXECUTE_PLAN_HPP
