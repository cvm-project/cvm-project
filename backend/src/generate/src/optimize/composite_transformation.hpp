#ifndef OPTIMIZE_COMPOSITE_TRANSFORMATION_HPP
#define OPTIMIZE_COMPOSITE_TRANSFORMATION_HPP

#include <string>
#include <vector>

#include "dag_transformation.hpp"

namespace optimize {

class CompositeTransformation : public DagTransformation {
public:
    explicit CompositeTransformation(std::vector<std::string> transformations)
        : transformations_(std::move(transformations)) {}
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "composite_transformation";
    }

private:
    const std::vector<std::string> transformations_;
};

}  // namespace optimize

#endif  // OPTIMIZE_COMPOSITE_TRANSFORMATION_HPP
