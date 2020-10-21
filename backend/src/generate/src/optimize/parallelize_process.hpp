#ifndef OPTIMIZE_PARALLELIZE_PROCESS_HPP
#define OPTIMIZE_PARALLELIZE_PROCESS_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ParallelizeProcess : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "parallelize_process";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_PROCESS_HPP
