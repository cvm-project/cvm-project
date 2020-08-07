#ifndef OPTIMIZE_VERIFY_HPP
#define OPTIMIZE_VERIFY_HPP

#include "dag_transformation.hpp"

namespace optimize {

class Verify : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override { return "verify"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_VERIFY_HPP
