#ifndef OPTIMIZE_MATERIALIZE_MULTIPLE_READS_HPP
#define OPTIMIZE_MATERIALIZE_MULTIPLE_READS_HPP

#include "dag_transformation.hpp"

namespace optimize {

class MaterializeMultipleReads : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    auto name() const -> std::string override {
        return "materialize_multiple_reads";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_MATERIALIZE_MULTIPLE_READS_HPP
