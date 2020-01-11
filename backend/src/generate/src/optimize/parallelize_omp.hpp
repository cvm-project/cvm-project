#ifndef OPTIMIZE_PARALLELIZE_OMP_HPP
#define OPTIMIZE_PARALLELIZE_OMP_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ParallelizeOmp : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    auto name() const -> std::string override { return "parallelize_omp"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_OMP_HPP
