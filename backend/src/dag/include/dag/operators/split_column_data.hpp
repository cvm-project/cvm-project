#ifndef DAG_OPERATORS_SPLIT_COLUMN_DATA_HPP
#define DAG_OPERATORS_SPLIT_COLUMN_DATA_HPP

#include "operator.hpp"

class DAGSplitColumnData : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGSplitColumnData, "split_column_data");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 2; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_SPLIT_COLUMN_DATA_HPP
