#ifndef DAG_OPERATORS_OPERATOR_HPP
#define DAG_OPERATORS_OPERATOR_HPP

#include <cstddef>

#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>
#include <polymorphic_value.h>

#include "dag/collection/attribute_id.hpp"
#include "dag/collection/tuple.hpp"
#include "utils/registry.hpp"

// cppcheck-suppress noConstructor
class DAGOperator {
public:
    isocpp_p0201::polymorphic_value<dag::collection::Tuple> tuple;
    std::set<std::shared_ptr<dag::AttributeId>> read_set;
    std::set<std::shared_ptr<dag::AttributeId>> write_set;
    std::string llvm_ir;
    size_t id{};

    virtual ~DAGOperator() = default;

    [[nodiscard]] virtual auto Clone() const -> DAGOperator * = 0;

    [[nodiscard]] virtual auto name() const -> std::string = 0;
    [[nodiscard]] virtual auto num_in_ports() const -> size_t = 0;
    [[nodiscard]] virtual auto num_out_ports() const -> size_t = 0;

    virtual void to_json(nlohmann::json *json) const {};
    virtual void from_json(const nlohmann::json &json){};

    auto HasInOutput(const dag::AttributeId *attribute) const -> bool;
    auto Reads(const dag::AttributeId *attribute) const -> bool;
    auto Writes(const dag::AttributeId *attribute) const -> bool;
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define JITQ_DAGOPERATOR(CLASS_NAME, OPERATOR_NAME)                           \
public:                                                                       \
    constexpr static const char *kName = OPERATOR_NAME;                       \
    static auto MakeDagOperator()->DAGOperator * { return new CLASS_NAME(); } \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                          \
    virtual auto Clone() const->CLASS_NAME *override {                        \
        static_assert(std::is_base_of<DAGOperator, CLASS_NAME>::value,        \
                      #CLASS_NAME " must be derived from DAGOperator");       \
        return new CLASS_NAME(*this);                                         \
    }                                                                         \
    auto name() const->std::string override { return kName; }

// NOLINTNEXTLINE(google-runtime-references)
void from_json(const nlohmann::json &json, DAGOperator &op);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const DAGOperator &op);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const DAGOperator *op);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const std::unique_ptr<DAGOperator> &op);

using make_dag_function = DAGOperator *(*)();  // function pointer type
using OperatorParserRegistry = utils::Registry<make_dag_function>;

auto MakeDAGOperator(const std::string &op_name) -> DAGOperator *;

#endif  // DAG_OPERATORS_OPERATOR_HPP
