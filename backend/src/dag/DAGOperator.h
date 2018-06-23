//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGOPERATOR_H
#define DAG_DAGOPERATOR_H

#include <cstddef>
#include <json.hpp>
#include <set>
#include <vector>

#include "dag/attribute_id.h"
#include "dag/collection/field.hpp"
#include "dag/collection/tuple.hpp"

// cppcheck-suppress noConstructor
class DAGOperator {
public:
    std::shared_ptr<dag::collection::Tuple> tuple;
    std::set<std::shared_ptr<dag::AttributeId>> read_set;
    std::set<std::shared_ptr<dag::AttributeId>> write_set;
    std::string llvm_ir;
    size_t id{};

    virtual ~DAGOperator() = default;

    virtual std::string name() const = 0;
    virtual size_t num_in_ports() const = 0;
    virtual size_t num_out_ports() const = 0;

    virtual void to_json(nlohmann::json *json) const = 0;
    virtual void from_json(const nlohmann::json &json) = 0;

    bool HasInOutput(const dag::AttributeId *attribute) const;
    bool Reads(const dag::AttributeId *attribute) const;
    bool Writes(const dag::AttributeId *attribute) const;
};

template <class OperatorType>
class DAGOperatorBase : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() { return new OperatorType(); }

    std::string name() const override { return OperatorType::kName; }
    size_t num_in_ports() const override { return OperatorType::kNumInPorts; }
    size_t num_out_ports() const override { return OperatorType::kNumOutPorts; }

    void to_json(nlohmann::json * /*json*/) const override {}
    void from_json(const nlohmann::json & /*json*/) override {}
};

// NOLINTNEXTLINE google-runtime-references
void from_json(const nlohmann::json &json, DAGOperator &op);
// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const DAGOperator &op);
// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const DAGOperator *op);
// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const std::unique_ptr<DAGOperator> &op);

#endif  // DAG_DAGOPERATOR_H
