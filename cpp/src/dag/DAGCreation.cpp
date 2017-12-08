
#include "DAGCreation.hpp"

#include <dirent.h>
#include <fstream>

#include "DAGCartesian.h"
#include "DAGCollection.h"
#include "DAGFilter.h"
#include "DAGJoin.h"
#include "DAGMap.h"
#include "DAGRange.h"
#include "DAGReduce.h"
#include "DAGReduceByKey.h"
#include "utils/utils.h"

using namespace std;

static DAGOperatorsMap opMap;

DAG *parse_dag(std::string dagstr) {
    load_operators();
    std::stringstream in(dagstr);
    DAG *dag = parse(&in);
    return dag;
};

void load_operators() {
    opMap.emplace(FILTER, &DAGFilter::make_dag_operator);
    opMap.emplace(RANGE, &DAGRange::make_dag_operator);
    opMap.emplace(COLLECTION, &DAGCollection::make_dag_operator);
    opMap.emplace(MAP, &DAGMap::make_dag_operator);
    opMap.emplace(JOIN, &DAGJoin::make_dag_operator);
    opMap.emplace(REDUCE, &DAGReduce::make_dag_operator);
    opMap.emplace(REDUCEBYKEY, &DAGReduceByKey::make_dag_operator);
    opMap.emplace(CARTESIAN, &DAGCartesian::make_dag_operator);
}

DAG *parse(std::stringstream *istream) {
    DAG *dag = new DAG;

    nlohmann::json j;
    *istream >> j;
    auto action = j[DAG_ACTION];
    dag->action = action;
    auto dag_list = j[DAG_DAG];
    vector<std::pair<DAGOperator *, vector<size_t> > > dag_ops(1);

    size_t op_count = 0;
    for (auto it = dag_list.begin(); it != dag_list.end(); it++) {
        size_t id = (*it)[DAG_ID];
        DAGOperator::lastOperatorIndex = id;
        std::string op_name = (*it)[DAG_OP];
        DAGOperator *op = get_operator(op_name);
        op->id = id;
        auto llvr_ir = (*it)[DAG_FUNC];
        if (!llvr_ir.is_null()) {
            op->llvm_ir = llvr_ir;
        }
        op->parse_json(*it);
        op->output_type = (*it)[DAG_OUTPUT_TYPE];
        op->fields = parse_output_type(op->output_type);
        vector<size_t> preds;
        auto preds_json = (*it)[DAG_PREDS];
        for (auto it_preds = preds_json.begin(); it_preds != preds_json.end();
             it_preds++) {
            preds.push_back((size_t)(*it_preds));
        }

        if (id >= dag_ops.size()) {
            dag_ops.resize(id * 2);
        }
        dag_ops[id] = std::pair<DAGOperator *, vector<size_t> >(op, preds);
        op_count++;
    }

    vector<bool> has_successors(op_count, 0);
    for (size_t i = 0; i < op_count; i++) {
        auto op = dag_ops[i].first;
        auto preds = dag_ops[i].second;
        for (auto it = preds.begin(); it != preds.end(); it++) {
            size_t pred_id = *it;
            auto predecessor = dag_ops[pred_id].first;
            op->predecessors.push_back(predecessor);
            predecessor->successors.push_back(op);
            has_successors[pred_id] = 1;
        }
    }

    for (size_t i = 0; i < op_count; i++) {
        if (!has_successors[i]) {
            dag->sink = dag_ops[i].first;
        }
    }

    return dag;
}

DAGOperator *get_operator(std::string opName) {
    if (!opMap.count(opName)) {
        throw std::invalid_argument("operator " + opName +
                                    " could not be found");
    }
    return opMap[opName]();
}

vector<TupleField> parse_output_type(const string &output) {
    string type_ = string_replace(output, "(", "");
    type_ = string_replace(type_, ")", "");
    type_ = string_replace(type_, " ", "");
    vector<string> types = split_string(type_, ",");
    vector<TupleField> ret;
    size_t pos = 0;
    for (auto t : types) {
        ret.push_back(TupleField(t, pos));
        pos++;
    }
    return ret;
}
