//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGOPERATOR_H
#define DAG_DAGOPERATOR_H

#include <cstddef>
#include <set>
#include <vector>

#include <json.hpp>

#include "dag/Column.h"
#include "dag/DAG.h"
#include "dag/TupleField.h"
#include "utils/constants.h"

class DAG;
class DAGVisitor;

// cppcheck-suppress noConstructor
class DAGOperator {
public:
    std::vector<DAGOperator *> predecessors;
    std::vector<DAGOperator *> successors;
    std::vector<TupleField> fields;
    std::set<Column *> read_set;
    std::set<Column *> write_set;
    std::set<TupleField> dead_set;
    std::string llvm_ir;
    std::string output_type;
    size_t id{};

    virtual ~DAGOperator() = default;

    virtual void Init(DAG *dag) { dag_ = dag; }

    // free only this operator
    void freeThisOperator() {
        predecessors.clear();
        successors.clear();
    }

    virtual std::string name() const = 0;

    virtual void accept(class DAGVisitor *v) = 0;
    virtual void to_json(nlohmann::json *json) const = 0;
    virtual void from_json(const nlohmann::json &json) = 0;

    bool writeSetContains(Column *c) {
        bool ret = false;
        for (auto col : write_set) {
            if (col->operator==(*c)) {
                ret = true;
                break;
            }
        }
        return ret;
    }

    DAG *dag() const {
        assert(dag_ != nullptr);
        return dag_;
    }

private:
    DAG *dag_{};
};

template <class OperatorType>
class DAGOperatorBase : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() { return new OperatorType(); }

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
