#ifndef DAG_OPERATORS_SPLIT_ROW_DATA_HPP
#define DAG_OPERATORS_SPLIT_ROW_DATA_HPP

#include "operator.hpp"

class DAGSplitRowData : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitRowData, "split_row_data");

public:
    size_t num_in_ports() const override { return 2; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_SPLIT_ROW_DATA_HPP
