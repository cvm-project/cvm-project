#ifndef OPTIMIZE_PARALLELIZE_LAMBDA_HPP
#define OPTIMIZE_PARALLELIZE_LAMBDA_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ParallelizeLambda : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "parallelize_lambda";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_LAMBDA_HPP
