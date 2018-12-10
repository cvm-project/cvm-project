#include "type_inference.hpp"

#include <stdexcept>

#include <boost/format.hpp>

#include "dag/DAG.h"
#include "dag/DAGOperators.h"
#include "dag/all_operator_declarations.hpp"
#include "dag/type/array.hpp"
#include "dag/type/atomic.hpp"
#include "dag/type/field_type.hpp"
#include "dag/type/tuple.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/visitor.hpp"

using dag::type::Array;
using dag::type::ArrayLayout;
using dag::type::Atomic;
using dag::type::FieldType;
using dag::type::Tuple;

const Tuple *ComputeOutputType(const DAG *const dag,
                               const DAGOperator *const op) {
    class TypeInferenceVisitor
        : public Visitor<TypeInferenceVisitor, const DAGOperator,
                         dag::AllOperatorTypes, const Tuple *> {
    public:
        explicit TypeInferenceVisitor(const DAG *const dag) : dag_(dag) {}

        const Tuple *operator()(
                const DAGAssertCorrectOpenNextClose *const op) const {
            return dag_->predecessor(op)->tuple->type;
        }

        const Tuple *operator()(const DAGCartesian *const op) const {
            auto const left_input_type = dag_->predecessor(op, 0)->tuple->type;
            auto const right_input_type = dag_->predecessor(op, 1)->tuple->type;

            auto output_fields = left_input_type->field_types;
            output_fields.insert(output_fields.end(),
                                 right_input_type->field_types.begin(),
                                 right_input_type->field_types.end());

            return Tuple::MakeTuple(output_fields);
        }

        const Tuple *operator()(const DAGRowScan *const op) const {
            auto const input_type = dag_->predecessor(op)->tuple->type;

            if (input_type->field_types.size() != 1) {
                throw std::invalid_argument(
                        "Input of RowScan operator must have a single "
                        "field");
            }

            auto const array_type =
                    dynamic_cast<const Array *>(input_type->field_types[0]);

            if (array_type == nullptr) {
                throw std::invalid_argument(
                        "Input of RowScan operator must have an Array "
                        "field");
            }

            auto field_types = array_type->tuple_type->field_types;
            if (op->add_index) {
                field_types.insert(field_types.begin(),
                                   Atomic::MakeAtomic("long"));
            }

            assert(array_type->number_dim == 1);
            assert(array_type->layout == ArrayLayout::kC);
            return Tuple::MakeTuple(field_types);
        }

        const Tuple *operator()(const DAGConstantTuple *const op) const {
            return op->tuple->type;
        }

        const Tuple *operator()(const DAGEnsureSingleTuple *const op) const {
            return dag_->predecessor(op)->tuple->type;
        }

        const Tuple *operator()(const DAGGroupBy *const op) const {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const partition_id_type = input_type->field_types[0];

            if (partition_id_type != Atomic::MakeAtomic("long")) {
                throw std::invalid_argument(
                        "First field of GroupBy must be 'long'");
            }
            auto const element_type =
                    Tuple::MakeTuple({input_type->field_types[1]});

            return Tuple::MakeTuple(
                    {partition_id_type,
                     Array::MakeArray(element_type, ArrayLayout::kC, 1)});
        }

        const Tuple *operator()(const DAGFilter *const op) const {
            return dag_->predecessor(op)->tuple->type;
        }

        const Tuple *operator()(const DAGJoin *const op) const {
            auto const left_input_type = dag_->predecessor(op, 0)->tuple->type;
            auto const right_input_type = dag_->predecessor(op, 1)->tuple->type;

            if (left_input_type->field_types.empty()) {
                throw std::invalid_argument(
                        "Left join input cannot be empty tuple");
            }

            if (right_input_type->field_types.empty()) {
                throw std::invalid_argument(
                        "Right join input cannot be empty tuple");
            }

            auto const left_key_type = left_input_type->field_types[0];
            auto const right_key_type = right_input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(left_key_type) ==
                nullptr) {
                throw std::invalid_argument(
                        "First field of left input of Join must be Atomic");
            }

            if (dynamic_cast<const dag::type::Atomic *>(right_key_type) ==
                nullptr) {
                throw std::invalid_argument(
                        "First field of right input of Join must be Atomic");
            }

            auto output_fields = left_input_type->field_types;
            output_fields.insert(output_fields.end(),
                                 right_input_type->field_types.begin() + 1,
                                 right_input_type->field_types.end());

            return Tuple::MakeTuple(output_fields);
        }

        const Tuple *operator()(const DAGMap *const op) const {
            return op->tuple->type;
        }

        const Tuple *operator()(const DAGMaterializeRowVector *const op) const {
            auto const element_type = dag_->predecessor(op)->tuple->type;

            return Tuple::MakeTuple(
                    {Array::MakeArray(element_type, ArrayLayout::kC, 1)});
        }

        const Tuple *operator()(const DAGParallelMap *const op) const {
            assert(dag_->has_inner_dag(op));
            auto const inner_dag = dag_->inner_dag(op);
            return inner_dag->output().op->tuple->type;
        }

        const Tuple *operator()(const DAGParameterLookup *const op) const {
            return op->tuple->type;
        }

        const Tuple *operator()(const DAGPartition *const op) const {
            auto const element_type = dag_->predecessor(op)->tuple->type;

            return Tuple::MakeTuple(
                    {Atomic::MakeAtomic("long"),
                     Array::MakeArray(element_type, ArrayLayout::kC, 1)});
        }

        const Tuple *operator()(const DAGPipeline *const op) const {
            assert(dag_->has_inner_dag(op));
            auto const inner_dag = dag_->inner_dag(op);
            return inner_dag->output().op->tuple->type;
        }

        const Tuple *operator()(const DAGProjection *const op) const {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const &input_field_types = input_type->field_types;

            std::vector<const FieldType *> output_field_types;
            for (auto const pos : op->positions) {
                if (pos >= input_field_types.size()) {
                    auto const num_input_fields =
                            std::to_string(input_field_types.size());
                    throw std::invalid_argument(
                            std::string() +
                            "Input of Projection operator has fewer fields "
                            "than it refers to.\n" +
                            "    Number of fields: " + num_input_fields + "\n" +
                            "    Field accessed:   " + std::to_string(pos));
                }

                output_field_types.emplace_back(input_field_types.at(pos));
            }

            return Tuple::MakeTuple(output_field_types);
        }

        const Tuple *operator()(const DAGRange *const op) const {
            auto const pred = dag_->predecessor(op);
            auto const input_type = pred->tuple->type;
            auto const element_type = input_type->field_types[0];
            auto const computed_output_type = Tuple::MakeTuple({element_type});

            if (input_type->field_types.size() != 3) {
                throw std::invalid_argument(
                        "Input of Range operator must be a triple");
            }

            for (auto const t : input_type->field_types) {
                // TODO(ingo): it must also be a triple of *numbers*
                if (t != element_type) {
                    throw std::invalid_argument(
                            "Input of Range operator must be a homogeneous "
                            "triple");
                }
            }

            if (dynamic_cast<const dag::type::Atomic *>(element_type) ==
                nullptr) {
                throw std::invalid_argument(
                        "Input of Range operator must be a triple of Atomics");
            }

            return computed_output_type;
        }

        const Tuple *operator()(const DAGReduce *const op) const {
            return dag_->predecessor(op)->tuple->type;
        }

        const Tuple *operator()(const DAGReduceByKey *const op) const {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const key_type = input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(key_type) == nullptr) {
                throw std::invalid_argument(
                        "First field of ReduceByKey must be Atomic");
            }

            return input_type;
        }

        const Tuple *operator()(const DAGReduceByKeyGrouped *const op) const {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const key_type = input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(key_type) == nullptr) {
                throw std::invalid_argument(
                        "First field of ReduceByKey must be Atomic");
            }

            return input_type;
        }

        const Tuple *operator()(const DAGSplitCollection *const op) const {
            return dag_->predecessor(op)->tuple->type;
        }

        const Tuple *operator()(const DAGSplitRange *const op) const {
            return dag_->predecessor(op)->tuple->type;
        }

        const Tuple *operator()(const DAGOperator *const op) const {
            throw std::invalid_argument(
                    "Cannot infer output type of unknown operator " +
                    op->name());
        }

    private:
        const DAG *const dag_;
    };

    TypeInferenceVisitor visitor(dag);
    return visitor.Visit(op);
}

void CheckOutputType(const DAGOperator *const op, const DAG *const dag) {
    auto const computed_output_type = ComputeOutputType(dag, op);
    auto const output_type = op->tuple->type;
    if (output_type != computed_output_type) {
        throw std::invalid_argument(
                (boost::format("Operator %1% (%2%) has the wrong output type:\n"
                               "   found:    %3%\n"
                               "   expected: %4%\n") %
                 op->name() % op->id % output_type->to_string() %
                 computed_output_type->to_string())
                        .str());
    }
}

void RecomputeOutputType(DAGOperator *const op, const DAG *const dag) {
    auto const computed_output_type = ComputeOutputType(dag, op);
    op->tuple = std::make_unique<dag::collection::Tuple>(
            make_raw(computed_output_type));
}

void SetInnerGraphInputTypes(DAGOperator *const op, const DAG *const dag) {
    if (!dag->has_inner_dag(op)) return;

    auto const inner_dag = dag->inner_dag(op);
    for (auto const input : inner_dag->inputs()) {
        auto const inner_op = input.second.op;
        auto const input_port = input.first;
        auto const input_type = dag->predecessor(op, input_port)->tuple->type;
        inner_op->tuple =
                std::make_unique<dag::collection::Tuple>(make_raw(input_type));
    }
}

namespace optimize {

void TypeInference::Run(DAG *const dag) const {
    if (only_check_) {
        // Check output type
        dag::utils::ApplyInReverseTopologicalOrderRecursively(dag,
                                                              CheckOutputType);
    } else {
        // Update output type
        dag::utils::ApplyInReverseTopologicalOrderRecursively(
                dag, SetInnerGraphInputTypes, RecomputeOutputType);
    }
}

}  // namespace optimize
