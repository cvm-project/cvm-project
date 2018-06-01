#include "attribute_id_tracking.hpp"

#include <unordered_set>

#include "IR_analyzer/LLVMParser.h"
#include "dag/DAGOperator.h"
#include "dag/DAGOperators.h"
#include "utils/DAGVisitor.h"

class AttributeIdTrackingVisitor : public DAGVisitor {
public:
    explicit AttributeIdTrackingVisitor(const DAG *const dag)
        : DAGVisitor(dag) {}

    void visit(DAGCollection * /*op*/) override {}
    void visit(DAGRange * /*op*/) override {}

    void visit(DAGFilter *const op) override {
        auto &input_fields = dag()->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < op->tuple->fields.size(); i++) {
            input_fields[i]->attribute_id()->AddField(
                    op->tuple->fields[i].get());
        }
    }

    void visit(DAGCartesian *const op) override {
        auto &left_input_fields = dag()->predecessor(op, 0)->tuple->fields;
        auto &right_input_fields = dag()->predecessor(op, 1)->tuple->fields;

        for (size_t i = 0; i < left_input_fields.size(); i++) {
            left_input_fields[i]->attribute_id()->AddField(
                    op->tuple->fields[i].get());
        }

        for (size_t i = 0; i < right_input_fields.size(); i++) {
            auto &field = op->tuple->fields[i + left_input_fields.size()];
            right_input_fields[i]->attribute_id()->AddField(field.get());
        }
    }

    void visit(DAGJoin *const op) override {
        auto left = dag()->predecessor(op, 0);
        auto right = dag()->predecessor(op, 1);

        // remap left input's key attribute_id to right input's one
        left->tuple->fields[0]->attribute_id()->MoveFields(
                right->tuple->fields[0]->attribute_id().get());

        auto &left_input_fields = dag()->predecessor(op, 0)->tuple->fields;
        auto &right_input_fields = dag()->predecessor(op, 1)->tuple->fields;

        for (size_t i = 0; i < left_input_fields.size(); i++) {
            left_input_fields[i]->attribute_id()->AddField(
                    op->tuple->fields[i].get());
        }

        for (size_t i = 1; i < right_input_fields.size(); i++) {
            auto &field = op->tuple->fields[i + left_input_fields.size() - 1];
            right_input_fields[i]->attribute_id()->AddField(field.get());
        }
    }

    void visit(DAGMap *const op) override {
        LLVMParser parser(op->llvm_ir);

        auto &input_fields = dag()->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            for (const auto pos : parser.get_output_positions(i)) {
                auto &output_field = op->tuple->fields[pos];
                input_fields[i]->attribute_id()->AddField(output_field.get());
                input_fields[i]->attribute_id()->AddField(output_field.get());
            }
        }
    }

    void visit(DAGReduce * /*op*/) override {}

    void visit(DAGReduceByKey *const op) override {
        auto &input_fields = dag()->predecessor(op)->tuple->fields;
        input_fields[0]->attribute_id()->AddField(op->tuple->fields[0].get());
    }

    void visit(DAGReduceByKeyGrouped *const op) override {
        auto &input_fields = dag()->predecessor(op)->tuple->fields;
        input_fields[0]->attribute_id()->AddField(op->tuple->fields[0].get());
    }
};

class DetermineReadSetVisitor : public DAGVisitor {
public:
    explicit DetermineReadSetVisitor(const DAG *const dag) : DAGVisitor(dag) {}

    void visit(DAGCollection * /*op*/) override {}
    void visit(DAGRange * /*op*/) override {}

    void visit(DAGFilter *const op) override {
        LLVMParser parser(op->llvm_ir);
        auto &input_fields = dag()->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            if (parser.is_argument_read(i)) {
                op->read_set.insert(input_fields[i]->attribute_id());
            }
        }
    }

    void visit(DAGJoin *const op) override {
        op->read_set.insert(op->tuple->fields[0]->attribute_id());
    }

    void visit(DAGMap *const op) override {
        LLVMParser parser(op->llvm_ir);
        auto &input_fields = dag()->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            if (parser.is_argument_read(i)) {
                op->read_set.insert(input_fields[i]->attribute_id());
            }
        }
    }

    void visit(DAGReduce *const op) override {
        for (auto const &field : op->tuple->fields) {
            op->read_set.insert(field->attribute_id());
        }
    }

    void visit(DAGReduceByKey *const op) override {
        for (auto const &field : op->tuple->fields) {
            op->read_set.insert(field->attribute_id());
        }
    }

    void visit(DAGReduceByKeyGrouped *const op) override {
        for (auto const &field : op->tuple->fields) {
            op->read_set.insert(field->attribute_id());
        }
    }

    void visit(DAGCartesian * /*op*/) override {}
};

void AttributeIdTracking::optimize() {
    // Track attribute_ids
    AttributeIdTrackingVisitor attribute_id_tracking_visitor(dag_);
    attribute_id_tracking_visitor.StartVisit();

    // Reset and recompute read sets
    for (const auto op : dag_->operators()) {
        op->read_set.clear();
    }

    DetermineReadSetVisitor determine_read_set_visitor(dag_);
    determine_read_set_visitor.StartVisit();

    // Determine write set
    for (const auto op : dag_->operators()) {
        op->write_set.clear();

        std::unordered_set<const dag::AttributeId *> input_attribute_ids;
        for (const auto flow : dag_->in_flows(op)) {
            const auto input_op = flow.source;
            for (const auto &field : input_op->tuple->fields) {
                input_attribute_ids.insert(field->attribute_id().get());
            }
        }

        for (const auto &field : op->tuple->fields) {
            if (input_attribute_ids.count(field->attribute_id().get()) == 0) {
                op->write_set.insert(field->attribute_id());
            }
        }
    }
}
