//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGOPERATOR_H
#define DAG_DAGOPERATOR_H

#include <cstddef>
#include <set>
#include <vector>

#include <json.hpp>

#include "dag/attribute_id.h"
#include "dag/field/Field.h"

class DAGVisitor;

// cppcheck-suppress noConstructor
class DAGOperator {
public:
    std::vector<Field> fields;
    std::set<AttributeId *> read_set;
    std::set<AttributeId *> write_set;
    std::string llvm_ir;
    std::string output_type;
    size_t id{};

    virtual ~DAGOperator() = default;

    virtual std::string name() const = 0;
    virtual size_t num_in_ports() const = 0;
    virtual size_t num_out_ports() const = 0;

    virtual void accept(class DAGVisitor *v) = 0;
    virtual void to_json(nlohmann::json *json) const = 0;
    virtual void from_json(const nlohmann::json &json) = 0;

    bool HasInOutput(const AttributeId *c) const;
    bool Reads(const AttributeId *c) const;
    bool Writes(const AttributeId *c) const;
};

template <class OperatorType>
class DAGOperatorBase : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() { return new OperatorType(); }

    std::string name() const override { return OperatorType::kName; }
    size_t num_in_ports() const override { return OperatorType::kNumInPorts; }
    size_t num_out_ports() const override { return OperatorType::kNumOutPorts; }

    void inline accept(DAGVisitor *v) override;

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

#include "utils/DAGVisitor.h"

#endif  // DAG_DAGOPERATOR_H
