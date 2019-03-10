#include "composite_transformation.hpp"

#include <json.hpp>

#include "dag/DAG.h"
#include "optimize/dag_transformation.hpp"

namespace optimize {

void CompositeTransformation::Run(DAG *const dag,
                                  const std::string &config) const {
    const auto jconfig = nlohmann::json::parse(config);
    for (auto const &name : transformations_) {
        auto const transformation =
                optimize::DagTransformationRegistry::at(name);
        nlohmann::json::json_pointer jptr("/" + name);
        transformation->Run(dag, jconfig.value(jptr, nlohmann::json{}).dump());
    }
}

}  // namespace optimize
