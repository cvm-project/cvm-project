#ifndef OPTIMIZE_ATTRIBUTEID_TRACKING_HPP
#define OPTIMIZE_ATTRIBUTEID_TRACKING_HPP

#include "dag_transformation.hpp"

namespace optimize {

class AttributeIdTracking : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    std::string name() const override { return "attribute_id_tracking"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_ATTRIBUTEID_TRACKING_HPP
