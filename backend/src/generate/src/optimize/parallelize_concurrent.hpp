#ifndef OPTIMIZE_PARALLELIZE_CONCURRENT_HPP
#define OPTIMIZE_PARALLELIZE_CONCURRENT_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ParallelizeConcurrent : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "parallelize_concurrent";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_CONCURRENT_HPP
