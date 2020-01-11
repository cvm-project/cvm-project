#ifndef OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP
#define OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class AssertCorrectOpenNextClose : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    auto name() const -> std::string override {
        return "assert_correct_open_next_close";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP
