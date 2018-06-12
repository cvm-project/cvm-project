#ifndef OPTIMIZE_PARALLELIZE_HPP
#define OPTIMIZE_PARALLELIZE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class Parallelize : public DagTransformation {
public:
    void Run(DAG *dag) const override;
    std::string name() const override { return "parallelize"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_HPP
