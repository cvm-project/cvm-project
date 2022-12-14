#include "dag/dag.hpp"

#include <cassert>

#include <algorithm>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/uniqued.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

#include "dag/operators/operator.hpp"

auto DAG::AddOperator(DAGOperator *const op, const VertexProperties &properties)
        -> Vertex {
    return AddOperator(op, last_operator_id() + 1, properties);
}

auto DAG::AddOperator(DAGOperator *const op, const size_t id,
                      const VertexProperties &properties) -> Vertex {
    assert(op != nullptr);

    op->id = id;
    auto ret = this->operator_ids_.insert(op->id);
    assert(ret.second == true);

    return add_vertex(properties, graph_);
}

auto DAG::AddOperator(DAGOperator *const op, DAG *const inner_dag) -> Vertex {
    return AddOperator(op, last_operator_id() + 1, inner_dag);
}

auto DAG::AddOperator(DAGOperator *const op, const size_t id,
                      DAG *const inner_dag) -> Vertex {
    std::shared_ptr<DAGOperator> op_ptr(op);
    std::shared_ptr<DAG> inner_dag_ptr(inner_dag);
    return AddOperator(op_ptr.get(), id,
                       VertexInnerDag(inner_dag_ptr, VertexOperator(op_ptr)));
}

auto DAG::ReplaceOperator(const DAGOperator *const old_op,
                          DAGOperator *const new_op) -> Vertex {
    assert(old_op != nullptr);
    assert(new_op != nullptr);

    std::shared_ptr<DAGOperator> new_op_ptr(new_op);

    const auto v = to_vertex(old_op);
    auto properties_map = boost::get(operator_t(), graph_);
    properties_map[v] = std::move(new_op_ptr);

    return v;
}

void DAG::RemoveOperator(const DAGOperator *const op) {
    const auto v = to_vertex(op);
    assert(boost::degree(v, graph_) == 0);
    assert(boost::distance(inputs(op)) == 0);
    assert(boost::distance(outputs(op)) == 0);
    boost::remove_vertex(v, graph_);
}

void DAG::MoveOperator(DAG *const other_dag, DAGOperator *const op) {
    const auto v = to_vertex(op);
    const auto properties_map = boost::get(boost::vertex_all_t(), graph_);
    const auto properties = properties_map[v];
    RemoveOperator(op);
    other_dag->AddOperator(op, properties);
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

auto DAG::AddFlow(const DAGOperator *const source, const FlowTip &target)
        -> Edge {
    return AddFlow(source, target.op, target.port);
}

auto DAG::AddFlow(const DAGOperator *const source, int source_port,
                  const FlowTip &target) -> Edge {
    return AddFlow(source, source_port, target.op, target.port);
}

auto DAG::AddFlow(const FlowTip &source, const DAGOperator *const target,
                  const int target_port) -> Edge {
    return AddFlow(source.op, source.port, target, target_port);
}

auto DAG::AddFlow(const FlowTip &source, const FlowTip &target) -> Edge {
    return AddFlow(source.op, source.port, target.op, target.port);
}

void DAG::RemoveFlow(const Edge &e) { boost::remove_edge(e, graph_); }

void DAG::Clear() {
    reset_inputs();
    reset_outputs();

    std::vector<Flow> temp_flows;
    boost::push_back(temp_flows, flows());
    for (auto const &f : temp_flows) {
        RemoveFlow(f);
    }

    std::vector<DAGOperator *> temp_operators;
    boost::push_back(temp_operators, operators());
    for (auto const &f : temp_operators) {
        RemoveOperator(f);
    }
}

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

auto DAG::HasCycle() const -> bool {
    for (auto *const op : operators()) {
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

struct TreeDetectionVisitor : public boost::default_bfs_visitor {
    explicit TreeDetectionVisitor(bool *const result) : result_(result) {
        *result_ = true;
    }
    template <typename Edge, typename Graph>
    void non_tree_edge(const Edge /*e*/, const Graph & /*g*/) {
        *result_ = false;
    }
    bool *const result_;
};

auto DAG::IsTree() const -> bool {
    const DAGOperator *root = nullptr;
    for (auto *const op : operators()) {
        if (out_degree(op) > 1) return false;
        if (out_degree(op) == 0) {
            if (root != nullptr) return false;
            root = op;
        }
    }
    bool is_tree = true;
    using ColorMap = std::unordered_map<Vertex, boost::default_color_type>;
    ColorMap bfs_colors;
    TreeDetectionVisitor vis(&is_tree);
    // NOLINTNEXTLINE(clang-analyzer-core.StackAddressEscape)
    boost::breadth_first_search(
            graph(), to_vertex(root),
            boost::visitor(vis).color_map(
                    boost::make_assoc_property_map(bfs_colors)));
    if (!is_tree) return false;

    // If a node is not in bfs_colors after the BFS, the graph consists of more
    // than one connected component and is hence not a tree.
    return bfs_colors.size() == boost::distance(operators());
}

auto DAG::operators() const -> OperatorRange {
    return boost::make_iterator_range(boost::vertices(graph_)) |
           boost::adaptors::transformed(VertexToOperatorFunc(this));
}

auto DAG::flows() const -> FlowRange {
    return boost::make_iterator_range(boost::edges(graph_)) |
           boost::adaptors::transformed(EdgeToFlowFunc(this));
}

auto DAG::contains(const DAGOperator *op) const -> bool {
    return boost::algorithm::any_of_equal(operators(), op);
}

auto DAG::to_operator(const Vertex &v) const -> DAGOperator * {
    const auto operator_map = boost::get(operator_t(), graph_);
    const auto op = operator_map[v];
    return op.get();
}

auto DAG::to_flow(const Edge &e) const -> DAG::Flow {
    const auto source_v = boost::source(e, graph_);
    const auto target_v = boost::target(e, graph_);
    auto *const source_op = to_operator(source_v);
    auto *const target_op = to_operator(target_v);

    const auto source_port_map = boost::get(source_port_t(), graph_);
    const auto target_port_map = boost::get(target_port_t(), graph_);
    const auto source_port = source_port_map[e];
    const auto target_port = target_port_map[e];

    return Flow{{source_op, source_port}, {target_op, target_port}, e};
}

auto DAG::to_vertex(const DAGOperator *const op) const -> Vertex {
    const auto operator_map = boost::get(operator_t(), graph_);
    const auto vr = boost::make_iterator_range(boost::vertices(graph_)) |
                    boost::adaptors::filtered([&](const auto &v) {
                        return operator_map[v].get() == op;
                    });
    assert(boost::distance(vr) == 1);
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

auto DAG::in_degree(const DAGOperator *op) const -> size_t {
    return boost::in_degree(to_vertex(op), graph_);
}

auto DAG::in_degree(const DAGOperator *op, int target_port) const -> size_t {
    return boost::distance(in_flows(op, target_port));
}

auto DAG::out_degree(const DAGOperator *op) const -> size_t {
    return boost::out_degree(to_vertex(op), graph_);
}

auto DAG::out_degree(const DAGOperator *op, int source_port) const -> size_t {
    return boost::distance(out_flows(op, source_port));
}

auto DAG::predecessor(const DAGOperator *const op) const -> DAGOperator * {
    return in_flow(op).source.op;
}

auto DAG::predecessor(const DAGOperator *const op, const int target_port) const
        -> DAGOperator * {
    return in_flow(op, target_port).source.op;
}

auto DAG::successor(const DAGOperator *const op) const -> DAGOperator * {
    return out_flow(op).target.op;
}

auto DAG::successor(const DAGOperator *const op, const int source_port) const
        -> DAGOperator * {
    return out_flow(op, source_port).target.op;
}

auto DAG::has_inner_dag(const DAGOperator *const op) const -> bool {
    return inner_dag(op) != nullptr;
}

auto DAG::inner_dag(const DAGOperator *const op) const -> DAG * {
    const auto inner_graph_map = boost::get(inner_graph_t(), graph_);
    const auto dag = inner_graph_map[to_vertex(op)];
    return dag.get();
}

void DAG::set_inner_dag(const DAGOperator *const op, DAG *const inner_dag) {
    const auto inner_graph_map = boost::get(inner_graph_t(), graph_);
    inner_graph_map[to_vertex(op)].reset(inner_dag);
}

void DAG::remove_inner_dag(const DAGOperator *const op) {
    set_inner_dag(op, nullptr);
}

auto DAG::in_degree() const -> size_t { return inputs_.size(); }

auto DAG::num_in_ports() const -> size_t {
    return boost::distance(boost::make_iterator_range(inputs_) |
                           boost::adaptors::transformed(
                                   [](auto const &it) { return it.first; }) |
                           boost::adaptors::uniqued);
}

auto DAG::inputs() const -> DAG::InputRange {
    return boost::make_iterator_range(inputs_.cbegin(), inputs_.cend());
}

auto DAG::inputs(int dag_input_port) const -> DAG::InputRangeTips {
    return boost::make_iterator_range(inputs_.equal_range(dag_input_port)) |
           boost::adaptors::transformed(ExtractFlowTipFunc());
}

auto DAG::inputs(const DAGOperator *const op) const
        -> DAG::InputRangeByOperator {
    return inputs() | boost::adaptors::filtered(FilterInputByOperatorFunc(op));
}

auto DAG::input() const -> DAG::FlowTip {
    assert(inputs_.size() == 1);
    return input(0);
}

auto DAG::input(const int dag_input_port) const -> DAG::FlowTip {
    auto const ret = inputs(dag_input_port);
    assert(boost::distance(ret) == 1);
    return ret.front();
}

auto DAG::input(const DAGOperator *const op) const
        -> std::pair<int, DAG::FlowTip> {
    auto const ret = inputs(op);
    assert(boost::distance(ret) == 1);
    return ret.front();
}

auto DAG::input_port(const DAGOperator *const op) const -> int {
    return input(op).first;
}

void DAG::set_input(const FlowTip &input) {
    inputs_.clear();
    add_input(0, input);
}

void DAG::set_input(DAGOperator *const op, const int operator_input_port) {
    set_input({op, operator_input_port});
}

void DAG::set_input(const int dag_input_port, const FlowTip &input) {
    reset_input(dag_input_port);
    add_input(dag_input_port, input);
}

void DAG::set_input(const int dag_input_port, DAGOperator *const op,
                    int const operator_input_port) {
    set_input(dag_input_port, {op, operator_input_port});
}

void DAG::add_input(const int dag_input_port, const FlowTip &input) {
    assert(to_operator(to_vertex(input.op)) == input.op);
    inputs_.emplace(dag_input_port, input);
}

void DAG::add_input(const int dag_input_port, DAGOperator *const op,
                    int const operator_input_port) {
    add_input(dag_input_port, {op, operator_input_port});
}

void DAG::reset_input(const int dag_input_port) {
    inputs_.erase(dag_input_port);
}

void DAG::reset_inputs() { inputs_.clear(); }

auto DAG::out_degree() const -> size_t { return outputs_.size(); }

auto DAG::num_out_ports() const -> size_t { return out_degree(); }

auto DAG::outputs() const -> DAG::OutputRange {
    return boost::make_iterator_range(outputs_.cbegin(), outputs_.cend());
}

auto DAG::outputs(const DAGOperator *op) const -> DAG::OutputRangeByOperator {
    return outputs() | boost::adaptors::filtered(FilterInputByOperatorFunc(op));
}

auto DAG::output() const -> DAG::FlowTip {
    assert(outputs_.size() == 1);
    return output(0);
}

auto DAG::output(const int dag_output_port) const -> DAG::FlowTip {
    return outputs_.at(dag_output_port);
}

void DAG::set_output(const FlowTip &output) {
    assert(outputs_.size() <= 1);
    outputs_.clear();
    set_output(0, output);
}

void DAG::set_output(DAGOperator *const op, const int operator_output_port) {
    set_output({op, operator_output_port});
}

void DAG::set_output(const int dag_output_port, const FlowTip &output) {
    assert(to_operator(to_vertex(output.op)) == output.op);
    outputs_[dag_output_port] = output;
}

void DAG::set_output(const int dag_output_port, DAGOperator *const op,
                     int const operator_output_port) {
    set_output(dag_output_port, {op, operator_output_port});
}

void DAG::reset_outputs() { outputs_.clear(); }

auto DAG::last_operator_id() const -> size_t {
    if (!operator_ids_.empty()) {
        return *(operator_ids_.rbegin());
    }
    return 0;
}

struct FlowTipHelper {
    size_t op_id;
    int port;
};

// NOLINTNEXTLINE(google-runtime-references)
void from_json(const nlohmann::json &json, FlowTipHelper &flow_tip) {
    flow_tip.op_id = json.at("op");
    flow_tip.port = json.at("port");
}

auto nlohmann::adl_serializer<std::unique_ptr<DAG>>::from_json(
        const nlohmann::json &json) -> std::unique_ptr<DAG> {
    std::unique_ptr<DAG> dag(new DAG);

    std::map<size_t, DAGOperator *> operators;
    std::unordered_map<DAGOperator *, std::vector<FlowTipHelper>> predecessors;

    for (const auto &it : json.at("operators")) {
        std::string op_name = it.at("op");
        DAGOperator *op = nullptr;
        {
            std::unique_ptr<DAGOperator> op_ptr(MakeDAGOperator(op_name));
            op = op_ptr.get();
            ::from_json(it, *op_ptr);
            dag->AddOperator(op_ptr.release(), op->id);
            auto const [_, has_inserted] = operators.emplace(op->id, op);
            assert(has_inserted);
        }

        if (it.count("inner_dag") > 0) {
            std::unique_ptr<DAG> inner_dag = it["inner_dag"];
            dag->set_inner_dag(op, inner_dag.release());
        }

        {
            std::vector<FlowTipHelper> flow_tips = it.at("predecessors");
            auto const [_, has_inserted] = predecessors.emplace(op, flow_tips);
            assert(has_inserted);
        }
    }

    for (const auto &op_val : operators) {
        DAGOperator *const op = op_val.second;
        auto const preds = predecessors.at(op);
        for (size_t i = 0; i < preds.size(); i++) {
            auto const pred = preds.at(i);
            auto *const pred_op = operators.at(pred.op_id);
            dag->AddFlow(pred_op, pred.port, op, i);
        }
    }

    int current_output_port = 0;
    for (const auto &it : json.at("outputs")) {
        const size_t op_id = it.at("op");
        const int port = it.at("port");
        dag->set_output(current_output_port++, operators.at(op_id), port);
    }

    for (const auto &it : json.at("inputs")) {
        const size_t op_id = it.at("op");
        const int op_port = it.at("op_port");
        const int dag_port = it.at("dag_port");
        dag->add_input(dag_port, operators.at(op_id), op_port);
    }

    return dag;
}

void to_json(nlohmann::json &json, DAG *const dag) {
    to_json(json, const_cast<const DAG *>(dag));
}

void to_json(nlohmann::json &json, const DAG *const dag) {
    auto jops = nlohmann::json::array();
    for (const auto &op : dag->operators()) {
        // Convert operator to JSON
        nlohmann::json jop(op);

        // Add predecessors
        auto jpreds = nlohmann::json::array();
        for (size_t i = 0; i < op->num_in_ports(); i++) {
            auto const in_flow = dag->in_flow(op, i);
            jpreds.push_back({{"op", in_flow.source.op->id},
                              {"port", in_flow.source.port}});
        }
        jop.emplace("predecessors", jpreds);

        // Add inner DAG
        if (dag->has_inner_dag(op)) {
            jop.emplace("inner_dag", dag->inner_dag(op));
        }

        // Add to DAG
        jops.push_back(jop);
    }
    json.emplace("operators", jops);

    auto inputs = json.emplace("inputs", nlohmann::json::array()).first;
    for (auto const input : dag->inputs()) {
        inputs->push_back({{"op", input.second.op->id},     //
                           {"op_port", input.second.port},  //
                           {"dag_port", input.first}});
    }

    auto outputs = json.emplace("outputs", nlohmann::json::array()).first;
    for (size_t i = 0; i < dag->out_degree(); i++) {
        auto const output = dag->output(i);
        outputs->push_back({{"op", output.op->id}, {"port", output.port}});
    }
}

void to_json(nlohmann::json &json, const std::unique_ptr<const DAG> &dag) {
    to_json(json, dag.get());
}

void nlohmann::adl_serializer<std::unique_ptr<DAG>>::to_json(
        nlohmann::json &json, const std::unique_ptr<DAG> &dag) {
    ::to_json(json, dag.get());
}

auto ParseDag(std::istream *istream) -> DAG * {
    nlohmann::json json;
    (*istream) >> json;
    auto dag = json.get<std::unique_ptr<DAG>>();

    return dag.release();
}
auto ParseDag(const std::string &dagstr) -> DAG * {
    std::stringstream stream(dagstr);
    return ParseDag(&stream);
}
