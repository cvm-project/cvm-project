#ifndef OPTIMIZE_PARALLELIZE_HPP
#define OPTIMIZE_PARALLELIZE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class Parallelize : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    auto name() const -> std::string override { return "parallelize"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_HPP
