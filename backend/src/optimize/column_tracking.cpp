#include "column_tracking.hpp"

#include <unordered_set>

#include "IR_analyzer/LLVMParser.h"
#include "dag/Column.h"
#include "dag/DAGOperator.h"
#include "dag/DAGOperators.h"
#include "utils/DAGVisitor.h"

class DetermineReadSetVisitor : public DAGVisitor {
public:
    explicit DetermineReadSetVisitor(const DAG *const dag) : DAGVisitor(dag) {}

    void visit(DAGCollection * /*op*/) override {}
    void visit(DAGRange * /*op*/) override {}

    void visit(DAGFilter *const op) override {
        LLVMParser parser(op->llvm_ir);
        auto &input_fields = dag()->predecessor(op)->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            if (parser.is_argument_read(i)) {
                op->read_set.insert(input_fields[i].column);
            }
        }
    }

    void visit(DAGJoin *const op) override {
        op->read_set.insert(op->fields[0].column);
    }

    void visit(DAGMap *const op) override {
        LLVMParser parser(op->llvm_ir);
        auto &input_fields = dag()->predecessor(op)->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            if (parser.is_argument_read(i)) {
                op->read_set.insert(input_fields[i].column);
            }
        }
    }

    void visit(DAGReduce *const op) override {
        for (auto const &field : op->fields) {
            op->read_set.insert(field.column);
        }
    }

    void visit(DAGReduceByKey *const op) override {
        for (auto const &field : op->fields) {
            op->read_set.insert(field.column);
        }
    }

    void visit(DAGReduceByKeyGrouped *const op) override {
        for (auto const &field : op->fields) {
            op->read_set.insert(field.column);
        }
    }

    void visit(DAGCartesian * /*op*/) override {}
};

void ColumnTracking::optimize() {
    // Reset and recompute read sets
    for (const auto op : dag_->operators()) {
        op->read_set.clear();
    }

    DetermineReadSetVisitor determine_read_set_visitor(dag_);
    determine_read_set_visitor.StartVisit();

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
