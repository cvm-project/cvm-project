#ifndef DAG_OPERATORS_SPLIT_ROW_DATA_HPP
#define DAG_OPERATORS_SPLIT_ROW_DATA_HPP

#include "operator.hpp"

class DAGSplitRowData : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitRowData, "split_row_data");

public:
    auto num_in_ports() const -> size_t override { return 2; }
    auto num_out_ports() const -> size_t override { return 1; }
};

#endif  // DAG_OPERATORS_SPLIT_ROW_DATA_HPP
