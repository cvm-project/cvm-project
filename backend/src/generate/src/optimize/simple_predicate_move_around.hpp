//
// Created by sabir on 29.08.17.
//

#ifndef OPTIMIZE_SIMPLE_PREDICATE_MOVE_AROUND_HPP
#define OPTIMIZE_SIMPLE_PREDICATE_MOVE_AROUND_HPP

#include "dag_transformation.hpp"

namespace optimize {

class SimplePredicateMoveAround : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    auto name() const -> std::string override {
        return "simple_predicate_move_around";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_SIMPLE_PREDICATE_MOVE_AROUND_HPP
