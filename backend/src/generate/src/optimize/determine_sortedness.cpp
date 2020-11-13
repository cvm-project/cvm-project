#include "determine_sortedness.hpp"

#include "dag/dag.hpp"
#include "dag/operators/all_operator_declarations.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/visitor.hpp"

using dag::collection::FL_GROUPED;
using dag::collection::FL_SORTED;
using dag::collection::FL_UNIQUE;

class DetermineSortednessVisitor
    : public Visitor<DetermineSortednessVisitor, DAGOperator,
                     dag::AllOperatorTypes> {
public:
    explicit DetermineSortednessVisitor(const DAG *const dag) : dag_(dag) {}

    void operator()(DAGCartesian *op) const {
        const auto &left_fields = dag_->predecessor(op, 0)->tuple->fields;
        const auto &right_fields = dag_->predecessor(op, 1)->tuple->fields;

        for (size_t i = 0; i < right_fields.size(); i++) {
            auto &field = op->tuple->fields[i + left_fields.size()];
            if (right_fields[i]->properties().count(FL_UNIQUE) > 0) {
                field->AddProperty(FL_GROUPED);
            }
            if (right_fields[i]->properties().count(FL_SORTED) > 0) {
                field->AddProperty(FL_SORTED);
            }
        }
    }

    void operator()(DAGColumnScan *op) const {
        if (op->add_index) {
            op->tuple->fields[0]->AddProperty(FL_UNIQUE);
            op->tuple->fields[0]->AddProperty(FL_SORTED);
        }
    }

    void operator()(DAGRowScan *op) const {
        if (op->add_index) {
            op->tuple->fields[0]->AddProperty(FL_UNIQUE);
            op->tuple->fields[0]->AddProperty(FL_SORTED);
        }
    }

    void operator()(DAGFilter *op) const {
        auto const &input_fields = dag_->predecessor(op)->tuple->fields;
        for (size_t i = 0; i < op->tuple->fields.size(); i++) {
            op->tuple->fields[i]->CopyProperties(*input_fields[i]);
        }
    }

    void operator()(DAGJoin *op) const {
        const auto &left_fields = dag_->predecessor(op, 0)->tuple->fields;
        const auto &right_fields = dag_->predecessor(op, 1)->tuple->fields;

        const bool is_left_unique =
                left_fields[0]->properties().count(FL_UNIQUE) > 0;
        const bool is_right_unique =
                right_fields[0]->properties().count(FL_UNIQUE) > 0;

        if (is_left_unique && is_right_unique) {
            for (size_t i = 0; i < left_fields.size(); i++) {
                if (left_fields[i]->properties().count(FL_UNIQUE) > 0) {
                    auto &field = op->tuple->fields[i];
                    field->AddProperty(FL_UNIQUE);
                }
            }
            for (size_t i = 1; i < right_fields.size(); i++) {
                if (right_fields[i]->properties().count(FL_UNIQUE) > 0) {
                    auto &field = op->tuple->fields[i + left_fields.size() - 1];
                    field->AddProperty(FL_UNIQUE);
                }
            }
        } else if (is_right_unique) {
            for (size_t i = 1; i < right_fields.size(); i++) {
                if (right_fields[i]->properties().count(FL_UNIQUE) > 0) {
                    auto &field = op->tuple->fields[i + left_fields.size() - 1];
                    field->AddProperty(FL_GROUPED);
                }
            }
        }
    }

    void operator()(DAGMap *op) const {
        auto const &input_fields = dag_->predecessor(op)->tuple->fields;
        for (const auto &field : op->tuple->fields) {
            auto const input_field_it =
                    std::find_if(input_fields.begin(), input_fields.end(),
                                 [&](auto const &other) {
                                     return *(field->attribute_id()) ==
                                            *(other->attribute_id());
                                 });
            if (input_field_it != input_fields.end()) {
                field->CopyProperties(**input_field_it);
            }
        }
    }

    void operator()(DAGRange *op) const {
        for (const auto &field : op->tuple->fields) {
            field->AddProperty(FL_UNIQUE);
            field->AddProperty(FL_SORTED);
        }
    }

    void operator()(DAGReduceByKey *op) const {
        op->tuple->fields[0]->AddProperty(FL_UNIQUE);
    }

    void operator()(DAGReduceByKeyGrouped *op) const {
        auto const &input_fields = dag_->predecessor(op)->tuple->fields;
        op->tuple->fields[0]->CopyProperties(*input_fields[0]);
        op->tuple->fields[0]->AddProperty(FL_UNIQUE);
    }

    void operator()(DAGOperator * /*op*/) const {}

private:
    const DAG *const dag_;
};

namespace optimize {

void DetermineSortedness::Run(DAG *const dag,
                              const std::string & /*config*/) const {
    // Clear all attributes
    for (auto *op : dag->operators()) {
        for (auto &field : op->tuple->fields) {
            field->ClearProperties();
        }
    }

    // Derive attributes from predecessors
    DetermineSortednessVisitor visitor(dag);
    dag::utils::ApplyInReverseTopologicalOrder(dag, visitor.functor());

    // 'grouped' is implied by 'unique' or 'sorted'
    for (auto *op : dag->operators()) {
        for (auto &field : op->tuple->fields) {
            if (field->properties().count(FL_UNIQUE) > 0) {
                field->AddProperty(FL_GROUPED);
            }
            if (field->properties().count(FL_SORTED) > 0) {
                field->AddProperty(FL_GROUPED);
            }
        }
    }
}

}  // namespace optimize
