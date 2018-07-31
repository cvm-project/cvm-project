#include "composite_transformation.hpp"

#include "dag/DAG.h"
#include "optimize/dag_transformation.hpp"

namespace optimize {

void CompositeTransformation::Run(DAG *const dag) const {
    for (auto const &name : transformations_) {
        auto const transformation =
                optimize::DagTransformationRegistry::transformation(name);
        transformation->Run(dag);
    }
}

}  // namespace optimize
