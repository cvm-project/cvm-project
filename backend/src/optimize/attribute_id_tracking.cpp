#include "attribute_id_tracking.hpp"

#include <unordered_set>

#include "dag/DAGOperator.h"
#include "dag/DAGOperators.h"
#include "dag/all_operator_declarations.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "llvm_helpers/function.hpp"
#include "utils/visitor.hpp"

class AttributeIdTrackingVisitor
    : public Visitor<AttributeIdTrackingVisitor, DAGOperator,
                     dag::AllOperatorTypes> {
public:
    explicit AttributeIdTrackingVisitor(const DAG *const dag) : dag_(dag) {}

    void operator()(DAGFilter *const op) const {
        auto &input_fields = dag_->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < op->tuple->fields.size(); i++) {
            input_fields[i]->attribute_id()->AddField(
                    op->tuple->fields[i].get());
        }
    }

    void operator()(DAGCartesian *const op) const {
        auto &left_input_fields = dag_->predecessor(op, 0)->tuple->fields;
        auto &right_input_fields = dag_->predecessor(op, 1)->tuple->fields;

        for (size_t i = 0; i < left_input_fields.size(); i++) {
            left_input_fields[i]->attribute_id()->AddField(
                    op->tuple->fields[i].get());
        }

        for (size_t i = 0; i < right_input_fields.size(); i++) {
            auto &field = op->tuple->fields[i + left_input_fields.size()];
            right_input_fields[i]->attribute_id()->AddField(field.get());
        }
    }

    void operator()(DAGJoin *const op) const {
        auto left = dag_->predecessor(op, 0);
        auto right = dag_->predecessor(op, 1);

        // remap left input's key attribute_id to right input's one
        left->tuple->fields[0]->attribute_id()->MoveFields(
                right->tuple->fields[0]->attribute_id().get());

        auto &left_input_fields = dag_->predecessor(op, 0)->tuple->fields;
        auto &right_input_fields = dag_->predecessor(op, 1)->tuple->fields;

        for (size_t i = 0; i < left_input_fields.size(); i++) {
            left_input_fields[i]->attribute_id()->AddField(
                    op->tuple->fields[i].get());
        }

        for (size_t i = 1; i < right_input_fields.size(); i++) {
            auto &field = op->tuple->fields[i + left_input_fields.size() - 1];
            right_input_fields[i]->attribute_id()->AddField(field.get());
        }
    }

    void operator()(DAGMap *const op) const {
        llvm_helpers::Function parser(op->llvm_ir);

        auto &input_fields = dag_->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            for (const auto pos : parser.ComputeOutputPositions(i)) {
                auto &output_field = op->tuple->fields[pos];
                input_fields[i]->attribute_id()->AddField(output_field.get());
                input_fields[i]->attribute_id()->AddField(output_field.get());
            }
        }
    }

    void operator()(DAGReduceByKey *const op) const {
        auto &input_fields = dag_->predecessor(op)->tuple->fields;
        input_fields[0]->attribute_id()->AddField(op->tuple->fields[0].get());
    }

    void operator()(DAGReduceByKeyGrouped *const op) const {
        auto &input_fields = dag_->predecessor(op)->tuple->fields;
        input_fields[0]->attribute_id()->AddField(op->tuple->fields[0].get());
    }

    void operator()(DAGOperator *const /*op*/) const {}

private:
    const DAG *const dag_;
};

class DetermineReadSetVisitor
    : public Visitor<DetermineReadSetVisitor, DAGOperator,
                     dag::AllOperatorTypes> {
public:
    explicit DetermineReadSetVisitor(const DAG *const dag) : dag_(dag) {}

    void operator()(DAGFilter *const op) const {
        llvm_helpers::Function parser(op->llvm_ir);
        auto &input_fields = dag_->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            if (parser.ComputeIsArgumentRead(i)) {
                op->read_set.insert(input_fields[i]->attribute_id());
            }
        }
    }

    void operator()(DAGJoin *const op) const {
        op->read_set.insert(op->tuple->fields[0]->attribute_id());
    }

    void operator()(DAGMap *const op) const {
        llvm_helpers::Function parser(op->llvm_ir);
        auto &input_fields = dag_->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < input_fields.size(); i++) {
            if (parser.ComputeIsArgumentRead(i)) {
                op->read_set.insert(input_fields[i]->attribute_id());
            }
        }
    }

    void operator()(DAGReduce *const op) const {
        for (auto const &field : op->tuple->fields) {
            op->read_set.insert(field->attribute_id());
        }
    }

    void operator()(DAGReduceByKey *const op) const {
        for (auto const &field : op->tuple->fields) {
            op->read_set.insert(field->attribute_id());
        }
    }

    void operator()(DAGReduceByKeyGrouped *const op) const {
        for (auto const &field : op->tuple->fields) {
            op->read_set.insert(field->attribute_id());
        }
    }

    void operator()(DAGOperator *const /*op*/) const {}

private:
    const DAG *const dag_;
};

void AttributeIdTracking::optimize() {
    // Track attribute_ids
    const AttributeIdTrackingVisitor attribute_id_tracking_visitor(dag_);
    dag::utils::ApplyInReverseTopologicalOrder(
            dag_, attribute_id_tracking_visitor.functor());

    // Reset and recompute read sets
    for (const auto op : dag_->operators()) {
        op->read_set.clear();
    }

    const DetermineReadSetVisitor determine_read_set_visitor(dag_);
    dag::utils::ApplyInReverseTopologicalOrder(
            dag_, determine_read_set_visitor.functor());

    // Determine write set
    for (const auto op : dag_->operators()) {
        op->write_set.clear();

        std::unordered_set<const dag::AttributeId *> input_attribute_ids;
        for (const auto flow : dag_->in_flows(op)) {
            const auto input_op = flow.source.op;
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
