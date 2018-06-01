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
        auto &input_fields = dag()->predecessor(op)->fields;
        for (size_t i = 0; i < op->fields.size(); i++) {
            op->fields[i].attribute_id_ = input_fields[i].attribute_id_;
        }
    }

    void visit(DAGCartesian *const op) override {
        auto &left_input_fields = dag()->predecessor(op, 0)->fields;
        auto &right_input_fields = dag()->predecessor(op, 1)->fields;

        for (size_t i = 0; i < left_input_fields.size(); i++) {
            op->fields[i].attribute_id_ = left_input_fields[i].attribute_id_;
        }

        for (size_t i = 0; i < right_input_fields.size(); i++) {
            auto &field = op->fields[i + left_input_fields.size()];
            field.attribute_id_ = right_input_fields[i].attribute_id_;
        }
    }

    void visit(DAGJoin *const op) override {
        auto left = dag()->predecessor(op, 0);
        auto right = dag()->predecessor(op, 1);

        // remap left input's key attribute_id to right input's one
        *(right->fields[0].attribute_id_) = *(left->fields[0].attribute_id_);
        left->fields[0].attribute_id_->addFields(
                right->fields[0].attribute_id_->fields());

        auto &left_input_fields = dag()->predecessor(op, 0)->fields;
        auto &right_input_fields = dag()->predecessor(op, 1)->fields;

        for (size_t i = 0; i < left_input_fields.size(); i++) {
            op->fields[i].attribute_id_ = left_input_fields[i].attribute_id_;
        }

        for (size_t i = 1; i < right_input_fields.size(); i++) {
            auto &field = op->fields[i + left_input_fields.size() - 1];
            field.attribute_id_ = right_input_fields[i].attribute_id_;
        }
    }

    void visit(DAGMap *const op) override {
        LLVMParser parser(op->llvm_ir);

        auto &input_fields = dag()->predecessor(op)->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            for (const auto pos : parser.get_output_positions(i)) {
                auto &output_field = op->fields[pos];
                output_field.attribute_id_ = input_fields[i].attribute_id_;
                input_fields[i].attribute_id_->addField(&(output_field));
            }
        }
    }

    void visit(DAGReduce * /*op*/) override {}

    void visit(DAGReduceByKey *const op) override {
        auto &input_fields = dag()->predecessor(op)->fields;
        op->fields[0].attribute_id_ = input_fields[0].attribute_id_;
    }

    void visit(DAGReduceByKeyGrouped *const op) override {
        auto &input_fields = dag()->predecessor(op)->fields;
        op->fields[0].attribute_id_ = input_fields[0].attribute_id_;
    }
};

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
                op->read_set.insert(input_fields[i].attribute_id_);
            }
        }
    }

    void visit(DAGJoin *const op) override {
        op->read_set.insert(op->fields[0].attribute_id_);
    }

    void visit(DAGMap *const op) override {
        LLVMParser parser(op->llvm_ir);
        auto &input_fields = dag()->predecessor(op)->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            if (parser.is_argument_read(i)) {
                op->read_set.insert(input_fields[i].attribute_id_);
            }
        }
    }

    void visit(DAGReduce *const op) override {
        for (auto const &field : op->fields) {
            op->read_set.insert(field.attribute_id_);
        }
    }

    void visit(DAGReduceByKey *const op) override {
        for (auto const &field : op->fields) {
            op->read_set.insert(field.attribute_id_);
        }
    }

    void visit(DAGReduceByKeyGrouped *const op) override {
        for (auto const &field : op->fields) {
            op->read_set.insert(field.attribute_id_);
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

        std::unordered_set<const AttributeId *> input_attribute_ids;
        for (const auto flow : dag_->in_flows(op)) {
            const auto input_op = flow.source;
            for (const auto &field : input_op->fields) {
                input_attribute_ids.insert(field.attribute_id_);
            }
        }

        for (const auto &field : op->fields) {
            if (input_attribute_ids.count(field.attribute_id_) == 0) {
                op->write_set.insert(field.attribute_id_);
            }
        }
    }
}
