#include "type_inference.hpp"

#include <stdexcept>

#include <boost/format.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <polymorphic_value.h>

#include "dag/dag.hpp"
#include "dag/operators/all_operator_declarations.hpp"
#include "dag/operators/all_operator_definitions.hpp"
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

auto ComputeOutputType(const DAG *const dag, const DAGOperator *const op)
        -> const Tuple * {
    class TypeInferenceVisitor
        : public Visitor<TypeInferenceVisitor, const DAGOperator,
                         dag::AllOperatorTypes, const Tuple *> {
    public:
        explicit TypeInferenceVisitor(const DAG *const dag) : dag_(dag) {}

    private:
        auto HandleAntiJoin(const DAGOperator *const op) const
                -> const Tuple * {
            auto const left_input_type = dag_->predecessor(op, 0)->tuple->type;
            auto const right_input_type = dag_->predecessor(op, 1)->tuple->type;

            if (left_input_type->field_types.empty()) {
                throw std::invalid_argument(
                        "Left anti-join input cannot be empty tuple");
            }

            if (right_input_type->field_types.empty()) {
                throw std::invalid_argument(
                        "Right anti-join input cannot be empty tuple");
            }

            auto const left_key_type = left_input_type->field_types[0];
            auto const right_key_type = right_input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(left_key_type) ==
                nullptr) {
                throw std::invalid_argument(
                        "First field of left input of anti-join must be "
                        "Atomic");
            }

            if (dynamic_cast<const dag::type::Atomic *>(right_key_type) ==
                nullptr) {
                throw std::invalid_argument(
                        "First field of right input of anti-join must be "
                        "Atomic");
            }

            return left_input_type;
        }

    public:
        auto operator()(const DAGAntiJoin *const op) const -> const Tuple * {
            return HandleAntiJoin(op);
        }

        auto operator()(const DAGAntiJoinPredicated *const op) const
                -> const Tuple * {
            return HandleAntiJoin(op);
        }

        auto operator()(const DAGAssertCorrectOpenNextClose *const op) const
                -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGBroadcast *const op) const -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGCartesian *const op) const -> const Tuple * {
            auto const left_input_type = dag_->predecessor(op, 0)->tuple->type;
            auto const right_input_type = dag_->predecessor(op, 1)->tuple->type;

            auto output_fields = left_input_type->field_types;
            output_fields.insert(output_fields.end(),
                                 right_input_type->field_types.begin(),
                                 right_input_type->field_types.end());

            return Tuple::MakeTuple(output_fields);
        }

        auto operator()(const DAGColumnScan *const op) const -> const Tuple * {
            auto const input_type = dag_->predecessor(op)->tuple->type;

            if (input_type->field_types.empty()) {
                throw std::invalid_argument(
                        "Input of ColumnScan operator must have at least one"
                        "field");
            }

            std::vector<const FieldType *> fields;

            for (const auto field_type : input_type->field_types) {
                auto const array_type = dynamic_cast<const Array *>(field_type);

                if (array_type == nullptr) {
                    throw std::invalid_argument(
                            "Input of ColumnScan operator must be a tuple of "
                            "arrays");
                }

                assert(array_type->num_dimensions == 1);
                assert(array_type->layout == ArrayLayout::kC);

                auto const field_types = array_type->tuple_type->field_types;

                if (field_types.size() != 1) {
                    throw std::invalid_argument(
                            "Input of ColumnScan operator must be a tuple of "
                            "arrays of atomics");
                }

                fields.push_back(field_types[0]);
            }

            if (op->add_index) {
                fields.insert(fields.begin(), Atomic::MakeAtomic("long"));
            }

            return Tuple::MakeTuple(fields);
        }

        auto operator()(const DAGCompiledPipeline *const op) const
                -> const Tuple * {
            return op->tuple->type;
        }

        auto operator()(const DAGRowScan *const op) const -> const Tuple * {
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

            assert(array_type->num_dimensions == 1);
            assert(array_type->layout == ArrayLayout::kC);
            return Tuple::MakeTuple(field_types);
        }

        auto operator()(const DAGConcurrentExecute *const op) const
                -> const Tuple * {
            assert(dag_->has_inner_dag(op));
            auto const inner_dag = dag_->inner_dag(op);
            return inner_dag->output().op->tuple->type;
        }

        auto operator()(const DAGConstantTuple *const op) const
                -> const Tuple * {
            return op->tuple->type;
        }

        auto operator()(const DAGEnsureSingleTuple *const op) const
                -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGExchange *const op) const -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGGroupBy *const op) const -> const Tuple * {
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

        auto operator()(const DAGFilter *const op) const -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGExpandPattern *const op) const
                -> const Tuple * {
            auto const left_input_type = dag_->predecessor(op, 0)->tuple->type;
            auto const right_input_type = dag_->predecessor(op, 1)->tuple->type;

            if (left_input_type->field_types.size() != 1) {
                throw std::invalid_argument(
                        "Left ExpandPattern input must have one field");
            }

            if (right_input_type->field_types.size() != 1) {
                throw std::invalid_argument(
                        "Right ExpandPattern input must have one field");
            }

            auto const left_field_type =
                    dynamic_cast<const dag::type::Atomic *>(
                            left_input_type->field_types[0]);
            auto const right_field_type =
                    dynamic_cast<const dag::type::Atomic *>(
                            right_input_type->field_types[0]);

            if (left_field_type == nullptr ||
                left_field_type->type != "std::string") {
                throw std::invalid_argument(
                        "Pattern input (left) of ExpandPattern must be a "
                        "string");
            }

            if (right_field_type == nullptr ||
                right_field_type->type != "long") {
                throw std::invalid_argument(
                        "Parameter input (right) of ExpandPattern must be a "
                        "long");
            }

            return Tuple::MakeTuple({Atomic::MakeAtomic("std::string")});
        }

        auto operator()(const DAGJoin *const op) const -> const Tuple * {
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

            for (int i = 0; i < op->num_keys; ++i) {
                auto const left_key_type = left_input_type->field_types[i];
                auto const right_key_type = right_input_type->field_types[i];

                if (dynamic_cast<const dag::type::Atomic *>(left_key_type) ==
                    nullptr) {
                    throw std::invalid_argument(
                            "Each key of left input of Join must be Atomic");
                }

                if (dynamic_cast<const dag::type::Atomic *>(right_key_type) ==
                    nullptr) {
                    throw std::invalid_argument(
                            "Each key of right input of Join must be "
                            "Atomic");
                }
            }

            auto output_fields = left_input_type->field_types;
            output_fields.insert(
                    output_fields.end(),
                    right_input_type->field_types.begin() + op->num_keys,
                    right_input_type->field_types.end());

            return Tuple::MakeTuple(output_fields);
        }

        auto operator()(const DAGMap *const op) const -> const Tuple * {
            return op->tuple->type;
        }

        auto operator()(const DAGMaterializeColumnChunks *const op) const
                -> const Tuple * {
            auto const input_type = dag_->predecessor(op)->tuple->type;

            std::vector<const FieldType *> column_types;
            for (auto const &type : input_type->field_types) {
                column_types.emplace_back(Array::MakeArray(
                        Tuple::MakeTuple({type}), ArrayLayout::kC, 1));
            }

            return Tuple::MakeTuple(column_types);
        }

        auto operator()(const DAGMaterializeParquet *const op) const
                -> const Tuple * {
            auto const conf_input_type = dag_->predecessor(op, 1)->tuple->type;
            auto const conf_type = dynamic_cast<const dag::type::Atomic *>(
                    conf_input_type->field_types.at(0));
            if (conf_type == nullptr || conf_type->type != "std::string") {
                throw std::invalid_argument(
                        "Configuration of MaterializeParquet must be string, "
                        "found: " +
                        conf_input_type->to_string());
            }

            auto const main_input_type = dag_->predecessor(op, 0)->tuple->type;

            for (auto const &field_type : main_input_type->field_types) {
                auto const array_type =
                        dynamic_cast<const dag::type::Array *>(field_type);
                if (array_type == nullptr) {
                    throw std::invalid_argument(
                            "Input of MaterializeParquet must be tuple of "
                            "Arrays, found: " +
                            main_input_type->to_string());
                }

                auto const inner_field_types =
                        array_type->tuple_type->field_types;
                if (inner_field_types.size() != 1) {
                    throw std::invalid_argument(
                            "Input of MaterializeParquet must be tuple of "
                            "Arrays of Atomics, found: " +
                            main_input_type->to_string());
                }

                auto const atomic_type =
                        dynamic_cast<const dag::type::Atomic *>(
                                inner_field_types.at(0));
                if (atomic_type == nullptr) {
                    throw std::invalid_argument(
                            "Input of MaterializeParquet must be tuple of "
                            "Arrays of Atomics, found: " +
                            main_input_type->to_string());
                }
            }

            return Tuple::MakeTuple({Atomic::MakeAtomic("std::string"),
                                     Atomic::MakeAtomic("long"),
                                     Atomic::MakeAtomic("long"),
                                     Atomic::MakeAtomic("long")});
        }

        auto operator()(const DAGMaterializeRowVector *const op) const
                -> const Tuple * {
            auto const element_type = dag_->predecessor(op)->tuple->type;

            return Tuple::MakeTuple(
                    {Array::MakeArray(element_type, ArrayLayout::kC, 1)});
        }

        auto operator()(const DAGNestedMap *const op) const -> const Tuple * {
            assert(dag_->has_inner_dag(op));
            auto const inner_dag = dag_->inner_dag(op);
            return inner_dag->output().op->tuple->type;
        }

        auto operator()(const DAGParallelMap *const op) const -> const Tuple * {
            assert(dag_->has_inner_dag(op));
            auto const inner_dag = dag_->inner_dag(op);
            return inner_dag->output().op->tuple->type;
        }

        auto operator()(const DAGParallelMapOmp *const op) const
                -> const Tuple * {
            assert(dag_->has_inner_dag(op));
            auto const inner_dag = dag_->inner_dag(op);
            return inner_dag->output().op->tuple->type;
        }

        auto operator()(const DAGParameterLookup *const op) const
                -> const Tuple * {
            return op->tuple->type;
        }

        auto operator()(const DAGParquetScan *const op) const -> const Tuple * {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const &input_field_types = input_type->field_types;

            if (input_field_types.size() != 4) {
                throw std::invalid_argument(
                        "Input of ParquetScan needs to have 4 fields "
                        "(file_path, slice_from, slice_to, num_slices).");
            }

            auto const file_path_field_type =
                    dynamic_cast<const Atomic *>(input_field_types.at(0));
            if (file_path_field_type == nullptr ||
                file_path_field_type->type != "std::string") {
                throw std::invalid_argument(
                        "file_path input of ParquetScan needs to be "
                        "'std::string'.");
            }

            for (size_t i = 1; i < input_field_types.size(); i++) {
                auto const atomic_type =
                        dynamic_cast<const Atomic *>(input_field_types.at(i));
                if (atomic_type == nullptr || atomic_type->type != "long") {
                    throw std::invalid_argument(
                            "Slice descriptor inputs of ParquetScan need to be "
                            "'long'.");
                }
            }

            return op->tuple->type;
        }

        auto operator()(const DAGPartition *const op) const -> const Tuple * {
            auto const conf_input_type = dag_->predecessor(op, 1)->tuple->type;
            auto const single_long_type =
                    Tuple::MakeTuple({Atomic::MakeAtomic("long")});

            if (conf_input_type != single_long_type) {
                throw std::invalid_argument(
                        "Input of Partition must be single long, found: " +
                        conf_input_type->to_string());
            }

            auto const element_type = dag_->predecessor(op, 0)->tuple->type;

            return Tuple::MakeTuple(
                    {Atomic::MakeAtomic("long"),
                     Array::MakeArray(element_type, ArrayLayout::kC, 1)});
        }

        auto operator()(const DAGPipeline *const op) const -> const Tuple * {
            assert(dag_->has_inner_dag(op));
            auto const inner_dag = dag_->inner_dag(op);
            return inner_dag->output().op->tuple->type;
        }

        auto operator()(const DAGProjection *const op) const -> const Tuple * {
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

        auto operator()(const DAGRange *const op) const -> const Tuple * {
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

        auto operator()(const DAGReduce *const op) const -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGReduceByKey *const op) const -> const Tuple * {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const key_type = input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(key_type) == nullptr) {
                throw std::invalid_argument(
                        "First field of ReduceByKey must be Atomic");
            }

            return input_type;
        }

        auto operator()(const DAGReduceByKeyGrouped *const op) const
                -> const Tuple * {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const key_type = input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(key_type) == nullptr) {
                throw std::invalid_argument(
                        "First field of ReduceByKeyGrouped must be Atomic");
            }

            return input_type;
        }

        auto operator()(const DAGReduceByIndex *const op) const
                -> const Tuple * {
            auto const input_type = dag_->predecessor(op)->tuple->type;
            auto const key_type = input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(key_type) == nullptr) {
                throw std::invalid_argument(
                        "First field of ReduceByIndex must be Atomic");
            }

            return input_type;
        }

        auto operator()(const DAGSemiJoin *const op) const -> const Tuple * {
            auto const left_input_type = dag_->predecessor(op, 0)->tuple->type;
            auto const right_input_type = dag_->predecessor(op, 1)->tuple->type;

            if (left_input_type->field_types.empty()) {
                throw std::invalid_argument(
                        "Left semijoin input cannot be empty tuple");
            }

            if (right_input_type->field_types.empty()) {
                throw std::invalid_argument(
                        "Right semijoin input cannot be empty tuple");
            }

            auto const left_key_type = left_input_type->field_types[0];
            auto const right_key_type = right_input_type->field_types[0];

            if (dynamic_cast<const dag::type::Atomic *>(left_key_type) ==
                nullptr) {
                throw std::invalid_argument(
                        "First field of left input of SemiJoin must be Atomic");
            }

            if (dynamic_cast<const dag::type::Atomic *>(right_key_type) ==
                nullptr) {
                throw std::invalid_argument(
                        "First field of right input of SemiJoin must be "
                        "Atomic");
            }

            return left_input_type;
        }

        auto operator()(const DAGSplitColumnData *const op) const
                -> const Tuple * {
            return dag_->predecessor(op, 0)->tuple->type;
        }

        auto operator()(const DAGSplitRowData *const op) const
                -> const Tuple * {
            return dag_->predecessor(op, 0)->tuple->type;
        }

        auto operator()(const DAGSplitRange *const op) const -> const Tuple * {
            return dag_->predecessor(op, 0)->tuple->type;
        }

        auto operator()(const DAGSort *const op) const -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGTopK *const op) const -> const Tuple * {
            return dag_->predecessor(op)->tuple->type;
        }

        auto operator()(const DAGZip *const op) const -> const Tuple * {
            std::vector<const FieldType *> output_field_types;
            for (size_t i = 0; i < op->num_in_ports(); i++) {
                auto const f = dag_->in_flow(op, i);
                boost::copy(f.source.op->tuple->type->field_types,
                            std::back_inserter(output_field_types));
            }

            return Tuple::MakeTuple(output_field_types);
        }

        auto operator()(const DAGOperator *const op) const -> const Tuple * {
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
    op->tuple = jbcoe::make_polymorphic_value<dag::collection::Tuple>(
            computed_output_type);
}

void SetInnerGraphInputTypes(DAGOperator *const op, const DAG *const dag) {
    if (!dag->has_inner_dag(op)) return;

    auto const inner_dag = dag->inner_dag(op);
    for (auto const input : inner_dag->inputs()) {
        auto const inner_op = input.second.op;
        auto const input_port = input.first;
        auto const input_type = dag->predecessor(op, input_port)->tuple->type;
        inner_op->tuple = jbcoe::make_polymorphic_value<dag::collection::Tuple>(
                input_type);
    }
}

namespace optimize {

void TypeInference::Run(DAG *const dag, const std::string & /*config*/) const {
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
