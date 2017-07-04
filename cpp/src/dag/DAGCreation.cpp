#include "DAGCreation.hpp"
#include "DAGFilter.h"
#include "DAGMap.h"
#include "DAGJoin.h"
#include "DAGRange.h"

using namespace std;

static DAGOperatorsMap opMap;

DAG *create_dag(std::string filename) {


    opMap.emplace(FILTER, &DAGFilter::make_dag_operator);
    opMap.emplace(RANGE, &DAGRange::make_dag_operator);
    opMap.emplace(MAP, &DAGMap::make_dag_operator);
    opMap.emplace(JOIN, &DAGJoin::make_dag_operator);

    DAG *dag = parse(std::ifstream(filename));
    dag->sink->make_operator()->printName();
    return dag;
};


DAG *parse(std::ifstream ifstream) {
    DAG *dag = new DAG;
    //read json file
    nlohmann::json j;
    ifstream >> j;
    auto action = j[DAG_ACTION];
    dag->action = action;
    auto dag_list = j[DAG_DAG];
    vector<std::pair<DAGOperator *, vector<size_t> > > dag_ops(1);

    size_t op_count = 0;
    for (auto it = dag_list.begin(); it != dag_list.end(); it++) {
        size_t id = (*it)[DAG_ID];
        std::string op_name = (*it)[DAG_OP];
        DAGOperator *op = get_operator(op_name);
        auto llvr_ir = (*it)[DAG_FUNC];
        if (!llvr_ir.is_null()) {
            op->llvm_ir = llvr_ir;
        }

        vector<size_t> preds;
        auto preds_json = (*it)[DAG_PREDS];
        for (auto it_preds = preds_json.begin(); it_preds != preds_json.end(); it_preds++) {
            preds.push_back((size_t) (*it_preds));
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
        for (auto it = preds.begin(); it != preds.end(); it++){
            size_t pred_id = *it;
            auto predecessor = dag_ops[pred_id].first;
            op->predecessors.push_back(predecessor);
            predecessor->successors.push_back(op);
            has_successors[pred_id] = 1;
        }
    }

    for (size_t i = 0; i < op_count; i++){
        if(!has_successors[i]){
            dag->sink = dag_ops[i].first;
        }
    }

    return dag;
}

DAGOperator *get_operator(std::string opName) {
    auto bla = opMap;
    return opMap[opName]();
}