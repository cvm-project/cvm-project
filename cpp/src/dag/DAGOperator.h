//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGOPERATOR_H
#define CPP_DAGOPERATOR_H

#include <cstddef>
#include <libs/json.hpp>
#include <set>
#include <vector>

#include "dag/Column.h"
#include "dag/TupleField.h"

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
    size_t id;
    static size_t lastOperatorIndex;

    virtual ~DAGOperator() {
        for (auto op : predecessors) {
            delete (op);
        }
    }

    //free only this operator
    void freeThisOperator() {
        predecessors.clear();
        successors.clear();
        delete (this);
    }

    virtual std::string get_name() { return "Operator_" + to_string(id); };

    virtual void accept(class DAGVisitor &v) =0;

    virtual void parse_json(nlohmann::json) {};

    bool writeSetContains(Column *c) {
        bool ret = 0;
        for (auto col:write_set) {
            if (col->operator==(*c)) {
                ret = 1;
                break;
            }
        }
        return ret;
    }
};

#endif //CPP_DAGOPERATOR_H
