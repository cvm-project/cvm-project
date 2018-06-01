#include "determine_sortedness.hpp"

#include "dag/DAGOperators.h"
#include "utils/DAGVisitor.h"

class DetermineSortednessVisitor : public DAGVisitor {
public:
    explicit DetermineSortednessVisitor(const DAG *const dag)
        : DAGVisitor(dag) {}

    void visit(DAGCartesian *op) override {
        const auto &left_fields = dag()->predecessor(op, 0)->fields;
        const auto &right_fields = dag()->predecessor(op, 1)->fields;

        if (op->stream_right) {
            for (size_t i = 0; i < right_fields.size(); i++) {
                auto &field = op->fields[i + left_fields.size()];
                if (right_fields[i].properties->count(FL_UNIQUE) > 0) {
                    field.properties->insert(FL_GROUPED);
                }
                if (right_fields[i].properties->count(FL_SORTED) > 0) {
                    field.properties->insert(FL_SORTED);
                }
            }
        } else {
            for (size_t i = 0; i < left_fields.size(); i++) {
                auto &field = op->fields[i];
                if (left_fields[i].properties->count(FL_UNIQUE) > 0) {
                    field.properties->insert(FL_GROUPED);
                }
                if (left_fields[i].properties->count(FL_SORTED) > 0) {
                    field.properties->insert(FL_SORTED);
                }
            }
        }
    }

    void visit(DAGCollection *op) override {
        if (op->add_index) {
            op->fields[0].properties->insert(FL_UNIQUE);
            op->fields[0].properties->insert(FL_SORTED);
        }
    }

    void visit(DAGFilter *op) override {
        auto const &input_fields = dag()->predecessor(op)->fields;
        for (size_t i = 0; i < op->fields.size(); i++) {
            op->fields[i].properties = input_fields[i].properties;
        }
    }

    void visit(DAGJoin *op) override {
        const auto &left_fields = dag()->predecessor(op, 0)->fields;
        const auto &right_fields = dag()->predecessor(op, 1)->fields;

        const bool is_left_unique =
                left_fields[0].properties->count(FL_UNIQUE) > 0;
        const bool is_right_unique =
                right_fields[0].properties->count(FL_UNIQUE) > 0;

        if (is_left_unique && is_right_unique) {
            for (size_t i = 0; i < left_fields.size(); i++) {
                auto &field = op->fields[i];
                if (left_fields[i].properties->count(FL_UNIQUE) > 0) {
                    field.properties->insert(FL_UNIQUE);
                }
            }
            for (size_t i = 0; i < right_fields.size(); i++) {
                auto &field = op->fields[i + left_fields.size() - 1];
                if (right_fields[i].properties->count(FL_UNIQUE) > 0) {
                    field.properties->insert(FL_UNIQUE);
                }
            }
        } else {
            if (is_left_unique && !op->stream_right) {
                for (size_t i = 0; i < left_fields.size(); i++) {
                    auto &field = op->fields[i];
                    if (left_fields[i].properties->count(FL_UNIQUE) > 0) {
                        field.properties->insert(FL_GROUPED);
                    }
                }
            } else if (is_right_unique && op->stream_right) {
                for (size_t i = 0; i < right_fields.size(); i++) {
                    auto &field = op->fields[i + left_fields.size() - 1];
                    if (right_fields[i].properties->count(FL_UNIQUE) > 0) {
                        field.properties->insert(FL_GROUPED);
                    }
                }
            }
        }
    }

    void visit(DAGMap *op) override {
        auto const &input_fields = dag()->predecessor(op)->fields;
        for (const auto &field : op->fields) {
            auto const input_field_it = std::find_if(
                    input_fields.begin(), input_fields.end(),
                    [&](auto const &other) {
                        return *(field.attribute_id_) == *(other.attribute_id_);
                    });
            if (input_field_it != input_fields.end()) {
                *(field.properties) = *(input_field_it->properties);
            }
        }
    }

    void visit(DAGRange *op) override {
        for (const auto &field : op->fields) {
            field.properties->insert(FL_UNIQUE);
            field.properties->insert(FL_SORTED);
        }
    }

    void visit(DAGReduceByKey *op) override {
        op->fields[0].properties->insert(FL_UNIQUE);
    }

    void visit(DAGReduceByKeyGrouped *op) override {
        auto const &input_fields = dag()->predecessor(op)->fields;
        op->fields[0].properties = input_fields[0].properties;
        op->fields[0].properties->insert(FL_UNIQUE);
    }

    void visit(DAGReduce * /*unused*/) override {}
};

void DetermineSortedness::optimize() {
    // Clear all attributes
    for (auto op : dag_->operators()) {
        for (auto &field : op->fields) {
            field.properties->clear();
        }
    }

    // Derive attributes from predecessors
    DetermineSortednessVisitor visitor(dag_);
    visitor.StartVisit();

    // 'grouped' is implied by 'unique' or 'sorted'
    for (auto op : dag_->operators()) {
        for (auto &field : op->fields) {
            if (field.properties->count(FL_UNIQUE) > 0) {
                field.properties->insert(FL_GROUPED);
            }
            if (field.properties->count(FL_SORTED) > 0) {
                field.properties->insert(FL_GROUPED);
            }
        }
    }
}
