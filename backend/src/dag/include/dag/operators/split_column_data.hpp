#ifndef DAG_OPERATORS_SPLIT_COLUMN_DATA_HPP
#define DAG_OPERATORS_SPLIT_COLUMN_DATA_HPP

#include "operator.hpp"

class DAGSplitColumnData : public DAGOperator {
    JITQ_DAGOPERATOR(DAGSplitColumnData, "split_column_data");

public:
    size_t num_in_ports() const override { return 1; }
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_OPERATORS_SPLIT_COLUMN_DATA_HPP
