#ifndef DAG_OPERATORS_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP
#define DAG_OPERATORS_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP

#include "operator.hpp"

class DAGAssertCorrectOpenNextClose : public DAGOperator {
    JITQ_DAGOPERATOR(DAGAssertCorrectOpenNextClose,
                     "assert_correct_open_next_close");

public:
    auto num_in_ports() const -> size_t override {
        return 1;
    }  // XXX: make this dynamic
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_ASSERT_CORRECT_OPEN_NEXT_CLOSE_HPP
