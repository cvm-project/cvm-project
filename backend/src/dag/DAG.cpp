#include "DAG.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <cassert>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "DAGCreation.hpp"
#include "DAGOperator.h"

auto DAG::AddOperator(DAGOperator *const op) -> Vertex {
    return AddOperator(op, last_operator_id() + 1);
}

auto DAG::AddOperator(DAGOperator *const op, const size_t id) -> Vertex {
    assert(op != nullptr);

    const std::shared_ptr<DAGOperator> op_ptr(op);
    op_ptr->id = id;
    auto ret = this->operator_ids_.insert(op_ptr->id);
    assert(ret.second == true);

    auto v = add_vertex(VertexOperator(op_ptr), graph_);

    return v;
}

void DAG::RemoveOperator(const DAGOperator *const op) {
    const auto v = to_vertex(op);
    assert(boost::degree(v, graph_) == 0);
    boost::remove_vertex(v, graph_);
}

auto DAG::AddFlow(const DAGOperator *const source,
                  const DAGOperator *const target, const int target_port)
        -> Edge {
    return AddFlow(source, 0, target, target_port);
}

auto DAG::AddFlow(const DAGOperator *const source, const int source_port,
                  const DAGOperator *const target, const int target_port)
        -> Edge {
    const auto ep = EdgeTargetPort(target_port, EdgeSourcePort(source_port));
    auto ret = add_edge(to_vertex(source), to_vertex(target), ep, graph_);
    assert(ret.second);
    assert(!HasCycle());
    return ret.first;
}

void DAG::RemoveFlow(const Edge &e) { boost::remove_edge(e, graph_); }

struct CycleDetectionVisitor : public boost::default_dfs_visitor {
    explicit CycleDetectionVisitor(bool *const result) : result_(result) {
        *result = false;
    }
    template <typename Edge, typename Graph>
    void back_edge(const Edge /*e*/, const Graph & /*g*/) {
        *result_ = true;
    }
    bool *const result_;
};

bool DAG::HasCycle() const {
    for (const auto op : operators()) {
        if (in_degree(op) == 0) {
            bool has_cycle = false;
            CycleDetectionVisitor vis(&has_cycle);
            boost::depth_first_search(
                    graph(), boost::visitor(vis).root_vertex(to_vertex(op)));
            if (has_cycle) return true;
        }
    }
    return false;
}

auto DAG::operators() const -> OperatorRange {
    return boost::make_iterator_range(boost::vertices(graph_)) |
           boost::adaptors::transformed(VertexToOperatorFunc(this));
}

auto DAG::flows() const -> FlowRange {
    return boost::make_iterator_range(boost::edges(graph_)) |
           boost::adaptors::transformed(EdgeToFlowFunc(this));
}

DAGOperator *DAG::to_operator(const Vertex &v) const {
    const auto operator_map = boost::get(operator_t(), graph_);
    const auto op = operator_map[v];
    return op.get();
}

DAG::Flow DAG::to_flow(const Edge &e) const {
    const auto source_v = boost::source(e, graph_);
    const auto target_v = boost::target(e, graph_);
    const auto source_op = to_operator(source_v);
    const auto target_op = to_operator(target_v);

    const auto source_port_map = boost::get(source_port_t(), graph_);
    const auto target_port_map = boost::get(target_port_t(), graph_);
    const auto source_port = source_port_map[e];
    const auto target_port = target_port_map[e];

    return Flow{source_op, target_op, source_port, target_port, e};
}

auto DAG::to_vertex(const DAGOperator *const op) const -> Vertex {
    const auto operator_map = boost::get(operator_t(), graph_);
    const auto vr = boost::make_iterator_range(boost::vertices(graph_)) |
                    boost::adaptors::filtered([&](const auto &v) {
                        return operator_map[v].get() == op;
                    });
    assert(distance(vr) == 1);
    return vr.front();
}

auto DAG::in_flows(const DAGOperator *const op) const -> InFlowRange {
    return boost::make_iterator_range(boost::in_edges(to_vertex(op), graph_)) |
           boost::adaptors::transformed(EdgeToFlowFunc(this));
}

auto DAG::out_flows(const DAGOperator *const op) const -> OutFlowRange {
    return boost::make_iterator_range(boost::out_edges(to_vertex(op), graph_)) |
           boost::adaptors::transformed(EdgeToFlowFunc(this));
}

auto DAG::in_flows(const DAGOperator *op, int target_port) const
        -> InFlowByTargetPortRange {
    return in_flows(op) |
           boost::adaptors::filtered(FilterFlowByTargetPortFunc(target_port));
}

auto DAG::out_flows(const DAGOperator *op, int source_port) const
        -> OutFlowBySourcePortRange {
    return out_flows(op) |
           boost::adaptors::filtered(FilterFlowBySourcePortFunc(source_port));
}

auto DAG::in_flow(const DAGOperator *const op) const -> Flow {
    const auto flows = in_flows(op);
    assert(in_degree(op) == 1);
    return flows.front();
}

auto DAG::in_flow(const DAGOperator *const op, const int target_port) const
        -> Flow {
    const auto flows = in_flows(op, target_port);
    assert(in_degree(op, target_port) == 1);
    return flows.front();
}

auto DAG::out_flow(const DAGOperator *const op) const -> Flow {
    const auto flows = out_flows(op);
    assert(out_degree(op) == 1);
    return flows.front();
}

auto DAG::out_flow(const DAGOperator *const op, const int source_port) const
        -> Flow {
    const auto flows = out_flows(op, source_port);
    assert(out_degree(op, source_port) == 1);
    return flows.front();
}

size_t DAG::in_degree(const DAGOperator *op) const {
    return boost::in_degree(to_vertex(op), graph_);
}

size_t DAG::in_degree(const DAGOperator *op, int target_port) const {
    return boost::distance(in_flows(op, target_port));
}

size_t DAG::out_degree(const DAGOperator *op) const {
    return boost::out_degree(to_vertex(op), graph_);
}

size_t DAG::out_degree(const DAGOperator *op, int source_port) const {
    return boost::distance(out_flows(op, source_port));
}

DAGOperator *DAG::predecessor(const DAGOperator *const op) const {
    return in_flow(op).source;
}

DAGOperator *DAG::predecessor(const DAGOperator *const op,
                              const int target_port) const {
    return in_flow(op, target_port).source;
}

DAGOperator *DAG::successor(const DAGOperator *const op) const {
    return out_flow(op).target;
}

DAGOperator *DAG::successor(const DAGOperator *const op,
                            const int source_port) const {
    return out_flow(op, source_port).target;
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
            dag->AddOperator(op_ptr.release(), op->id);
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
            dag->AddFlow(predecessor, op, &pred_id - &*preds.begin());
        }
    }

    for (const auto &op_val : operators) {
        DAGOperator *const op = op_val.second;
        if (dag->out_degree(op) == 0) {
            assert(dag->sink == nullptr);
            dag->sink = op;
        }
    }

    return dag;
}

void nlohmann::adl_serializer<std::unique_ptr<DAG>>::to_json(
        nlohmann::json &json, const std::unique_ptr<DAG> &dag) {
    auto jops = nlohmann::json::array();
    for (const auto &op : dag->operators()) {
        // Convert operator to JSON
        nlohmann::json jop(op);

        // Add predecessors
        auto jpreds = nlohmann::json::array();
        for (const auto &f : dag->in_flows(op)) {
            jpreds.push_back(f.source->id);
        }
        jop.emplace("predecessors", jpreds);

        // Add to DAG
        jops.push_back(jop);
    }
    json.emplace("dag", jops);
}
