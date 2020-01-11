#ifndef OPTIMIZE_GROUPED_REDUCE_BY_KEY_HPP
#define OPTIMIZE_GROUPED_REDUCE_BY_KEY_HPP

#include "dag_transformation.hpp"

namespace optimize {

class GroupedReduceByKey : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "grouped_reduce_by_key";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_GROUPED_REDUCE_BY_KEY_HPP
