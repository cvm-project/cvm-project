#ifndef OPTIMIZE_CANONICALIZE_HPP
#define OPTIMIZE_CANONICALIZE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class Canonicalize : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "canonicalize";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_CANONICALIZE_HPP
