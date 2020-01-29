#ifndef OPTIMIZE_PARALLELIZE_CONCURRENT_SINGLE_INOUT_HPP
#define OPTIMIZE_PARALLELIZE_CONCURRENT_SINGLE_INOUT_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ParallelizeConcurrentSingleInout : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "parallelize_concurrent_single_inout";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_CONCURRENT_SINGLE_INOUT_HPP
