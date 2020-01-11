#ifndef OPTIMIZE_TYPE_INFERENCE_HPP
#define OPTIMIZE_TYPE_INFERENCE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class TypeInference : public DagTransformation {
public:
    explicit TypeInference(const bool only_check = false)
        : only_check_(only_check) {}

    void Run(DAG *dag, const std::string &config) const override;
    auto name() const -> std::string override { return "type_inference"; }

private:
    const bool only_check_;
};

}  // namespace optimize

#endif  // OPTIMIZE_TYPE_INFERENCE_HPP
