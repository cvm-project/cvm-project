#ifndef RUNTIME_EXECUTE_PLAN_HPP
#define RUNTIME_EXECUTE_PLAN_HPP

#include <memory>
#include <string>

#include "dag/dag.hpp"

namespace runtime {

auto RegisterPlan(std::unique_ptr<const DAG> dag) -> size_t;

auto DumpDag(size_t plan_id) -> std::string;

auto ExecutePlan(const DAG* dag, const std::string& inputs_str) -> std::string;
auto ExecutePlan(size_t plan_id, const std::string& inputs_str) -> std::string;

}  // namespace runtime

#endif  // RUNTIME_EXECUTE_PLAN_HPP
