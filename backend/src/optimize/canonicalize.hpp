#ifndef OPTIMIZE_CANONICALIZE_HPP
#define OPTIMIZE_CANONICALIZE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class Canonicalize : public DagTransformation {
public:
    void Run(DAG *dag) const override;
    std::string name() const override { return "canonicalize"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_CANONICALIZE_HPP
