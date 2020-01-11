#ifndef OPTIMIZE_ATTRIBUTE_ID_TRACKING_HPP
#define OPTIMIZE_ATTRIBUTE_ID_TRACKING_HPP

#include "dag_transformation.hpp"

namespace optimize {

class AttributeIdTracking : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "attribute_id_tracking";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_ATTRIBUTE_ID_TRACKING_HPP
