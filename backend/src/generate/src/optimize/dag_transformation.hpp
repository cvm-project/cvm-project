#ifndef OPTIMIZE_DAG_TRANSFORMATION_HPP
#define OPTIMIZE_DAG_TRANSFORMATION_HPP

#include <string>

#include "utils/registry.hpp"

class DAG;

namespace optimize {

class DagTransformation {
public:
    virtual ~DagTransformation() = default;
    virtual void Run(DAG *dag, const std::string &config) const = 0;
    [[nodiscard]] virtual auto name() const -> std::string = 0;
};

using DagTransformationRegistry = utils::Registry<const DagTransformation>;

void LoadDagTransformations();

}  // namespace optimize

#endif  // OPTIMIZE_DAG_TRANSFORMATION_HPP
