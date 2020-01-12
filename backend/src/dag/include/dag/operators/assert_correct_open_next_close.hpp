#ifndef DAG_OPERATORS_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP
#define DAG_OPERATORS_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP

#include "operator.hpp"

class DAGAssertCorrectOpenNextClose : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGAssertCorrectOpenNextClose,
                     "assert_correct_open_next_close");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override {
        return 1;
    }  // XXX: make this dynamic
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP
