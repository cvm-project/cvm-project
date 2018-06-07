#include "column_tracking.hpp"

#include <unordered_set>

#include "dag/Column.h"
#include "dag/DAGOperator.h"

void ColumnTracking::optimize() {
    // Determine write set
    for (const auto op : dag_->operators()) {
        op->write_set.clear();

        std::unordered_set<const Column *> input_columns;
        for (const auto flow : dag_->in_flows(op)) {
            const auto input_op = flow.source;
            for (const auto &field : input_op->fields) {
                input_columns.insert(field.column);
            }
        }

        for (const auto &field : op->fields) {
            if (input_columns.count(field.column) == 0) {
                op->write_set.insert(field.column);
            }
        }
    }
}
