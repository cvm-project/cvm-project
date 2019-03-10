#ifndef OPTIMIZE_CREATE_PIPELINES_HPP
#define OPTIMIZE_CREATE_PIPELINES_HPP

#include "dag_transformation.hpp"

namespace optimize {

class CreatePipelines : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    std::string name() const override { return "create_pipelines"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_CREATE_PIPELINES_HPP
