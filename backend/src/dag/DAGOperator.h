//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGOPERATOR_H
#define DAG_DAGOPERATOR_H

#include <cstddef>
#include <nlohmann/json.hpp>
#include <set>
#include <type_traits>
#include <vector>

#include <jbcoe/polymorphic_value.h>

#include "attribute_id.h"
#include "collection/field.hpp"
#include "collection/tuple.hpp"
#include "utils/registry.hpp"

// cppcheck-suppress noConstructor
class DAGOperator {
public:
    jbcoe::polymorphic_value<dag::collection::Tuple> tuple;
    std::set<std::shared_ptr<dag::AttributeId>> read_set;
    std::set<std::shared_ptr<dag::AttributeId>> write_set;
    std::string llvm_ir;
    size_t id{};

    virtual ~DAGOperator() = default;

    virtual DAGOperator *Clone() const = 0;

    virtual std::string name() const = 0;
    virtual size_t num_in_ports() const = 0;
    virtual size_t num_out_ports() const = 0;

    virtual void to_json(nlohmann::json *json) const {};
    virtual void from_json(const nlohmann::json &json){};

    bool HasInOutput(const dag::AttributeId *attribute) const;
    bool Reads(const dag::AttributeId *attribute) const;
    bool Writes(const dag::AttributeId *attribute) const;
};

#define JITQ_DAGOPERATOR(CLASS_NAME, OPERATOR_NAME)                     \
public:                                                                 \
    constexpr static const char *kName = OPERATOR_NAME;                 \
    static DAGOperator *MakeDagOperator() { return new CLASS_NAME(); }  \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                    \
    virtual CLASS_NAME *Clone() const override {                        \
        static_assert(std::is_base_of<DAGOperator, CLASS_NAME>::value,  \
                      #CLASS_NAME " must be derived from DAGOperator"); \
        return new CLASS_NAME(*this);                                   \
    }                                                                   \
    std::string name() const override { return kName; }

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

DAGOperator *MakeDAGOperator(const std::string &op_name);

#endif  // DAG_DAGOPERATOR_H
