#include "DAG.h"

#include <unordered_map>
#include <vector>

#include <cassert>

#include "DAGCreation.hpp"
#include "DAGOperator.h"

void DAG::addOperator(DAGOperator *const op) {
    addOperator(op, last_operator_id() + 1);
}

void DAG::addOperator(DAGOperator *const op, const size_t id) {
    op->id = id;
    auto ret = this->operator_ids_.insert(op->id);
    assert(ret.second == true);

    this->operators_.emplace_back(op);
}

size_t DAG::last_operator_id() const {
    if (!operator_ids_.empty()) {
        return *(operator_ids_.rbegin());
    }
    return 0;
}

std::unique_ptr<DAG> nlohmann::adl_serializer<std::unique_ptr<DAG>>::from_json(
        const nlohmann::json &json) {
    std::unique_ptr<DAG> dag(new DAG);

    std::unordered_map<size_t, DAGOperator *> operators;
    std::unordered_map<DAGOperator *, std::vector<size_t>> predecessors;

    for (auto &it : json.at("dag")) {
        std::string op_name = it.at("op");
        DAGOperator *op = nullptr;
        {
            std::unique_ptr<DAGOperator> op_ptr(get_operator(op_name));
            op = op_ptr.get();
            ::from_json(it, *op_ptr);
            dag->addOperator(op_ptr.release(), op->id);
        }
        operators.emplace(op->id, op);

        std::vector<size_t> preds;
        auto preds_json = it.at("predecessors");
        for (auto &it_preds : preds_json) {
            preds.push_back(static_cast<size_t>(it_preds));
        }
        predecessors.emplace(op, preds);
    }

    for (const auto &op_val : operators) {
        DAGOperator *const op = op_val.second;
        auto preds = predecessors.at(op);
        for (const auto &pred_id : preds) {
            auto predecessor = operators.at(pred_id);
            op->predecessors.push_back(predecessor);
            predecessor->successors.push_back(op);
        }
    }

    for (const auto &op_val : operators) {
        DAGOperator *const op = op_val.second;
        if (op->successors.empty()) {
            assert(dag->sink == nullptr);
            dag->sink = op;
        }
    }

    return dag;
}

void nlohmann::adl_serializer<std::unique_ptr<DAG>>::to_json(
        nlohmann::json &json, const std::unique_ptr<DAG> &dag) {
    json.emplace("dag", dag->operators());
}
