#include "canonicalize.hpp"

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/format.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/sort.hpp>

#include "dag/DAG.h"
#include "dag/DAGOperator.h"
#include "dag/utils/apply_visitor.hpp"

using VertexLabelMap = std::unordered_map<const DAGOperator *, std::string>;
using ReverseLabelMap = std::map<std::string, const DAGOperator *>;

void Canonicalize::optimize() {
    // Assign deterministic label based on outoing path
    VertexLabelMap outgoing_path_label;
    dag::utils::ApplyInTopologicalOrder(
            dag_,  //
            [&](const DAGOperator *const op) {
                std::string label;
                for (auto const f : dag_->out_flows(op)) {
                    auto const successor_label =
                            outgoing_path_label.at(f.target.op);
                    label = std::max(label, (boost::format("%1%#%2%#%3%") %
                                             successor_label % f.source.port %
                                             f.target.port)
                                                    .str());
                }
                auto const ret = outgoing_path_label.emplace(op, label);
                assert(ret.second);
            });

    // Assign deterministic label based on incoming path
    VertexLabelMap incoming_path_label;
    dag::utils::ApplyInReverseTopologicalOrder(
            dag_,  //
            [&](const DAGOperator *const op) {
                std::string label;
                for (auto const f : dag_->in_flows(op)) {
                    auto const predecessor_label =
                            incoming_path_label.at(f.source.op);
                    label = std::max(label, (boost::format("%1%#%2%#%3%") %
                                             predecessor_label % f.source.port %
                                             f.target.port)
                                                    .str());
                }
                auto const ret = incoming_path_label.emplace(op, label);
                assert(ret.second);
            });

    // Compute unique label (=combination of incoming and outgoing path labels)
    ReverseLabelMap sorted_labels;
    for (auto const op : dag_->operators()) {
        auto const label =
                incoming_path_label[op] + "_" + outgoing_path_label[op];
        sorted_labels.emplace(label, op);
    }
    assert(sorted_labels.size() == incoming_path_label.size());
    assert(sorted_labels.size() == outgoing_path_label.size());

    // Determine new IDs and assign them
    std::unordered_map<const DAGOperator *, size_t> vertex_ids;
    for (auto const &it : sorted_labels) {
        vertex_ids.emplace(it.second, vertex_ids.size());
    }

    for (auto const op : dag_->operators()) {
        op->id = vertex_ids.at(op);
    }

    // Make copy of DAG components
    std::vector<DAGOperator *> temp_ops;
    std::vector<DAG::Flow> temp_flows;
    std::vector<std::pair<int, DAG::FlowTip>> temp_inputs;
    std::vector<std::pair<int, DAG::FlowTip>> temp_outputs;

    boost::copy(dag_->operators(), std::back_inserter(temp_ops));
    boost::copy(dag_->flows(), std::back_inserter(temp_flows));
    boost::copy(dag_->inputs(), std::back_inserter(temp_inputs));
    boost::copy(dag_->outputs(), std::back_inserter(temp_outputs));

    // Sort operators and flows
    boost::sort(temp_ops, [](const auto op1, const auto op2) {
        return op1->id < op2->id;
    });
    boost::sort(temp_flows, [](const auto &f1, const auto &f2) {
        return std::tie(f1.source.op->id, f1.source.port, f1.target.op->id,
                        f1.target.port) <
               std::tie(f2.source.op->id, f2.source.port, f2.target.op->id,
                        f2.target.port);
    });

    // Unset DAG componenets except operators
    dag_->reset_inputs();
    dag_->reset_outputs();
    for (auto const f : temp_flows) {
        dag_->RemoveFlow(f);
    }

    // Move out operators -- this resets their IDs!
    DAG temp_dag;
    for (auto const op : temp_ops) {
        dag_->MoveOperator(&temp_dag, op);
    }

    // Add back everything (in deterministic order)
    for (auto const op : temp_ops) {
        temp_dag.MoveOperator(dag_, op);
    }

    for (auto const &f : temp_flows) {
        dag_->AddFlow(f.source.op, f.source.port, f.target.op, f.target.port);
    }

    for (auto const &input : temp_inputs) {
        dag_->add_input(input.first, input.second);
    }

    for (auto const &output : temp_outputs) {
        dag_->set_output(output.first, output.second);
    }

    // Reassign recomputed IDs (they where reset by the moving)
    for (auto const op : dag_->operators()) {
        op->id = vertex_ids.at(op);
    }

    // Recurse
    for (auto const op : dag_->operators()) {
        if (dag_->has_inner_dag(op)) {
            Canonicalize inner_canon(dag_->inner_dag(op));
            inner_canon.optimize();
        }
    }
}
