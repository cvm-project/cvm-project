#include "canonicalize.hpp"

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/format.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/sort.hpp>

#include "dag/dag.hpp"
#include "dag/operators/operator.hpp"
#include "dag/utils/apply_visitor.hpp"

using VertexLabelMap = std::unordered_map<const DAGOperator *, std::string>;
using ReverseLabelMap = std::map<std::string, const DAGOperator *>;

namespace optimize {

void Canonicalize::Run(DAG *const dag, const std::string &config) const {
    // Recurse
    for (auto *const op : dag->operators()) {
        if (dag->has_inner_dag(op)) {
            // Canonicalize inner DAG
            auto *const inner_dag = dag->inner_dag(op);
            Run(inner_dag, config);

            // Bring inputs in pre-defined order

            // Remember outer-level in-flows
            std::vector<DAG::Flow> temp_in_flows;
            for (auto const &f : dag->in_flows(op)) {
                temp_in_flows.emplace_back(f);
            }

            // Remember inner-level inputs
            std::vector<std::pair<DAG::FlowTip, std::vector<DAG::FlowTip>>>
                    temp_inputs;

            for (size_t i = 0; i < op->num_in_ports(); i++) {
                assert(temp_inputs.size() == i);

                const auto in_flow = dag->in_flow(op, i);
                temp_inputs.emplace_back(in_flow.source,
                                         std::vector<DAG::FlowTip>());

                for (auto const input : inner_dag->inputs(i)) {
                    temp_inputs.back().second.emplace_back(input);
                }
            }

            // Remove all of them
            inner_dag->reset_inputs();
            for (auto const &f : temp_in_flows) {
                dag->RemoveFlow(f);
            }

            // Order them by inner-level consumers
            std::vector<decltype(temp_inputs)::value_type> temp_inputs_sorted(
                    temp_inputs.begin(), temp_inputs.end());

            // Order the consumers of the same input by (ID, port)
            for (auto &it : temp_inputs_sorted) {
                boost::sort(it.second, [](const auto &i1, const auto &i2) {
                    return std::tie(i1.op->id, i1.port) <
                           std::tie(i2.op->id, i2.port);
                });
            }

            // Order the inputs by smallest (ID, port) of consumers
            boost::sort(temp_inputs_sorted, [](const auto &i1, const auto &i2) {
                return std::tie(i1.second[0].op->id, i1.second[0].port) <
                       std::tie(i2.second[0].op->id, i2.second[0].port);
            });

            // Insert them again (in deterministic order)
            for (size_t i = 0; i < temp_inputs_sorted.size(); i++) {
                auto const &inputs = temp_inputs_sorted[i];
                dag->AddFlow(inputs.first.op, inputs.first.port, op, i);
                for (const auto &input : inputs.second) {
                    inner_dag->add_input(i, input);
                }
            }
        }
    }

    // Assign deterministic label based on outoing path
    VertexLabelMap outgoing_path_label;
    dag::utils::ApplyInTopologicalOrder(
            dag,  //
            [&](const DAGOperator *const op) {
                std::string label;
                for (auto const f : dag->out_flows(op)) {
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
            dag,  //
            [&](const DAGOperator *const op) {
                std::string label;
                for (auto const f : dag->in_flows(op)) {
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
    for (auto *const op : dag->operators()) {
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

    for (auto *const op : dag->operators()) {
        op->id = vertex_ids.at(op);
    }

    // Make copy of DAG components
    std::vector<DAGOperator *> temp_ops;
    std::vector<DAG::Flow> temp_flows;
    std::vector<std::pair<int, DAG::FlowTip>> temp_inputs;
    std::vector<std::pair<int, DAG::FlowTip>> temp_outputs;

    boost::copy(dag->operators(), std::back_inserter(temp_ops));
    boost::copy(dag->flows(), std::back_inserter(temp_flows));
    boost::copy(dag->inputs(), std::back_inserter(temp_inputs));
    boost::copy(dag->outputs(), std::back_inserter(temp_outputs));

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

    // Unset DAG components except operators
    dag->reset_inputs();
    dag->reset_outputs();
    for (auto const f : temp_flows) {
        dag->RemoveFlow(f);
    }

    // Move out operators -- this resets their IDs!
    DAG temp_dag;
    for (auto *const op : temp_ops) {
        dag->MoveOperator(&temp_dag, op);
    }

    // Add back everything (in deterministic order)
    for (auto *const op : temp_ops) {
        temp_dag.MoveOperator(dag, op);
    }

    for (auto const &f : temp_flows) {
        dag->AddFlow(f.source.op, f.source.port, f.target.op, f.target.port);
    }

    for (auto const &input : temp_inputs) {
        dag->add_input(input.first, input.second);
    }

    for (auto const &output : temp_outputs) {
        dag->set_output(output.first, output.second);
    }

    // Reassign recomputed IDs (they were reset by the moving)
    for (auto *const op : dag->operators()) {
        op->id = vertex_ids.at(op);
    }
}

}  // namespace optimize
