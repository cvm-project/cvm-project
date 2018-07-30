#ifndef OPTIMIZE_GROUPEDREDUCEBYKEY_HPP
#define OPTIMIZE_GROUPEDREDUCEBYKEY_HPP

#include "dag_transformation.hpp"

namespace optimize {

class GroupedReduceByKey : public DagTransformation {
public:
    void Run(DAG *dag) const override;
    std::string name() const override { return "grouped_reduce_by_key"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_GROUPEDREDUCEBYKEY_HPP
