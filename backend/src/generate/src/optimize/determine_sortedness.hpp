#ifndef OPTIMIZE_DETERMINE_SORTEDNESS_HPP
#define OPTIMIZE_DETERMINE_SORTEDNESS_HPP

#include "dag_transformation.hpp"

namespace optimize {

class DetermineSortedness : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "determine_sortedness";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_DETERMINE_SORTEDNESS_HPP
