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
#include "dag/TupleField.h"
#include "utils/constants.h"

class DAG;
class DAGVisitor;

class DAGOperator {
public:
    DAG *const dag_;
    std::vector<DAGOperator *> predecessors;
    std::vector<DAGOperator *> successors;
    std::vector<TupleField> fields;
    std::set<Column *> read_set;
    std::set<Column *> write_set;
    std::set<TupleField> dead_set;
    std::string llvm_ir;
    std::string output_type;
    size_t id{};

    explicit DAGOperator(DAG *const dag) : dag_(dag) {}
    virtual ~DAGOperator() = default;

    // free only this operator
    void freeThisOperator() {
        predecessors.clear();
        successors.clear();
    }

    virtual std::string name() const = 0;

    virtual void accept(class DAGVisitor *v) = 0;

    virtual void parse_json(const nlohmann::json & /*json*/) {}

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
};

#endif  // DAG_DAGOPERATOR_H
