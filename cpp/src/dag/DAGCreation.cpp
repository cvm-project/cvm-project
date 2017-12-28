
#include "DAGCreation.hpp"

#include <fstream>
#include <vector>

#include <dirent.h>

#include "DAGCartesian.h"
#include "DAGCollection.h"
#include "DAGFilter.h"
#include "DAGJoin.h"
#include "DAGMap.h"
#include "DAGRange.h"
#include "DAGReduce.h"
#include "DAGReduceByKey.h"
#include "utils/utils.h"

// TODO(sabir): Implement proper singleton pattern
static DAGOperatorsMap opMap;  // NOLINT

DAG *parse_dag(const std::string &dagstr) {
    load_operators();
    std::stringstream in(dagstr);
    DAG *dag = parse(&in);
    return dag;
};

template <class OperatorType>
void load_operator() {
    std::string name(OperatorType::kName);
    opMap.emplace(name.c_str(), &OperatorType::make_dag_operator);
}

void load_operators() {
    load_operator<DAGCartesian>();
    load_operator<DAGCollection>();
    load_operator<DAGFilter>();
    load_operator<DAGJoin>();
    load_operator<DAGMap>();
    load_operator<DAGRange>();
    load_operator<DAGReduce>();
    load_operator<DAGReduceByKey>();
}

DAG *parse(std::stringstream *istream) {
    auto *dag = new DAG;

    nlohmann::json j;
    *istream >> j;
    auto action = j[DAG_ACTION];
    // cppcheck-suppress uninitdata
    dag->action = action;
    auto dag_list = j[DAG_DAG];
    std::vector<std::pair<DAGOperator *, std::vector<size_t> > > dag_ops(1);

    size_t op_count = 0;
    for (auto &it : dag_list) {
        size_t id = it[DAG_ID];
        dag->last_operator_id = id;
        std::string op_name = it[DAG_OP];
        DAGOperator *op = get_operator(op_name, dag);
        dag->operators.emplace_back(op);
        op->id = id;
        auto llvr_ir = it[DAG_FUNC];
        if (!llvr_ir.is_null()) {
            op->llvm_ir = llvr_ir;
        }
        op->parse_json(it);
        op->output_type = it[DAG_OUTPUT_TYPE];
        op->fields = parse_output_type(op->output_type);
        std::vector<size_t> preds;
        auto preds_json = it[DAG_PREDS];
        for (auto &it_preds : preds_json) {
            preds.push_back(static_cast<size_t>(it_preds));
        }

        if (id >= dag_ops.size()) {
            dag_ops.resize(id * 2);
        }
        dag_ops[id] = std::pair<DAGOperator *, std::vector<size_t> >(op, preds);
        op_count++;
    }

    std::vector<bool> has_successors(op_count, false);
    for (size_t i = 0; i < op_count; i++) {
        auto op = dag_ops[i].first;
        auto preds = dag_ops[i].second;
        for (auto pred_id : preds) {
            auto predecessor = dag_ops[pred_id].first;
            op->predecessors.push_back(predecessor);
            predecessor->successors.push_back(op);
            has_successors[pred_id] = true;
        }
    }

    for (size_t i = 0; i < op_count; i++) {
        if (!has_successors[i]) {
            // cppcheck-suppress uninitdata
            dag->sink = dag_ops[i].first;
        }
    }

    return dag;
}

DAGOperator *get_operator(const std::string &opName, DAG *const dag) {
    if (opMap.count(opName) == 0) {
        throw std::invalid_argument("operator " + opName +
                                    " could not be found");
    }
    return opMap[opName](dag);
}

std::vector<TupleField> parse_output_type(const std::string &output) {
    std::string type_ = string_replace(output, "(", "");
    type_ = string_replace(type_, ")", "");
    type_ = string_replace(type_, " ", "");
    std::vector<std::string> types = split_string(type_, ",");
    std::vector<TupleField> ret;
    size_t pos = 0;
    for (auto t : types) {
        ret.emplace_back(t, pos);
        pos++;
    }
    return ret;
}
