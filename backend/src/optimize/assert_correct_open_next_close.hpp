#ifndef OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_H
#define OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_H

#include "dag_transformation.hpp"

namespace optimize {

class AssertCorrectOpenNextClose : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    std::string name() const override {
        return "assert_correct_open_next_close";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_H
