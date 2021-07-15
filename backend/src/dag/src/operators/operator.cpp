#include "dag/operators/operator.hpp"

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/mpl/for_each.hpp>
#include <polymorphic_value.h>

#include "dag/operators/all_operator_declarations.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/type/array.hpp"
#include "dag/type/tuple.hpp"
#include "utils/raw_ptr.hpp"

using dag::collection::Tuple;

void from_json(const nlohmann::json &json, DAGOperator &op) {
    op.id = json["id"].get<size_t>();
    if (json.count("func") > 0) {
        op.llvm_ir = json["func"];
    }
    auto const type =
            json.at("output_type").get<raw_ptr<const dag::type::Tuple>>();
    op.tuple = isocpp_p0201::make_polymorphic_value<Tuple>(type.get());
    op.from_json(json);
}

void to_json(nlohmann::json &json, const DAGOperator &op) {
    json.emplace("id", op.id);
    if (!op.llvm_ir.empty()) {
        json.emplace("func", op.llvm_ir);
    }
    json.emplace("output_type", op.tuple->type);
    json.emplace("op", op.name());
    op.to_json(&json);
}

void to_json(nlohmann::json &json, const DAGOperator *op) {
    to_json(json, *op);
}

void to_json(nlohmann::json &json, const std::unique_ptr<DAGOperator> &op) {
    to_json(json, *op);
}

auto DAGOperator::HasInOutput(const dag::AttributeId *const attribute) const
        -> bool {
    return boost::algorithm::any_of(tuple->fields, [&](auto const &f) {
        return *(f->attribute_id()) == *attribute;
    });
}

auto DAGOperator::Reads(const dag::AttributeId *attribute) const -> bool {
    return boost::algorithm::any_of(
            read_set, [&](auto const &col) { return *col == *attribute; });
}

auto DAGOperator::Writes(const dag::AttributeId *attribute) const -> bool {
    return boost::algorithm::any_of(
            write_set, [&](auto const &col) { return *col == *attribute; });
}

struct LoadOperatorFunctor {
    template <class OperatorType>
    void operator()(OperatorType * /*unused*/) {
        const std::string name(OperatorType::kName);
        OperatorParserRegistry::Register(
                name, std::make_unique<make_dag_function>(
                              &OperatorType::MakeDagOperator));
    }
};

void LoadOperators() {
    static bool has_loaded = false;
    if (has_loaded) return;

    boost::mpl::for_each<dag::AllOperatorPointerTypes>(LoadOperatorFunctor{});

    has_loaded = true;
}

auto MakeDAGOperator(const std::string &op_name) -> DAGOperator * {
    LoadOperators();
    return (*OperatorParserRegistry::at(op_name))();
}
