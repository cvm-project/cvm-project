#ifndef OPTIMIZE_COMPILE_INNER_PLANS_HPP
#define OPTIMIZE_COMPILE_INNER_PLANS_HPP

#include "dag_transformation.hpp"

namespace optimize {

class CompileInnerPlans : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "compile_inner_plans";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_COMPILE_INNER_PLANS_HPP
