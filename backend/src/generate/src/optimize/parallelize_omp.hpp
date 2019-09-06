#ifndef OPTIMIZE_PARALLELIZE_OMP_HPP
#define OPTIMIZE_PARALLELIZE_OMP_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ParallelizeOmp : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    std::string name() const override { return "parallelize_omp"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_OMP_HPP
