#ifndef OPTIMIZE_MATERIALIZE_MULTIPLE_READS_H
#define OPTIMIZE_MATERIALIZE_MULTIPLE_READS_H

#include "dag_transformation.hpp"

namespace optimize {

class MaterializeMultipleReads : public DagTransformation {
public:
    void Run(DAG *dag) const override;
    std::string name() const override { return "materialize_multiple_reads"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_MATERIALIZE_MULTIPLE_READS_H
