#ifndef DAG_DAG_HPP
#define DAG_DAG_HPP

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <nlohmann/json.hpp>

#include "collection/attribute_id.hpp"

class DAG;
class DAGOperator;

namespace nlohmann {
template <>
struct adl_serializer<std::unique_ptr<DAG>>;
}  // namespace nlohmann

/*
 * Install (Boost) properties attached to the (BGL) graph
 * (This is not part of the public interface.)
 */
struct operator_t {
    using kind = boost::vertex_property_tag;
};

struct inner_graph_t {
    using kind = boost::vertex_property_tag;
};

struct source_port_t {
    using kind = boost::edge_property_tag;
};

struct target_port_t {
    using kind = boost::edge_property_tag;
};

namespace boost {
enum vertex_operator_t { vertex_operator };
enum vertex_inner_graph_t { vertex_inner_graph };
enum edge_source_port_t { edge_source_port };
enum edge_target_port_t { edge_target_port };

BOOST_INSTALL_PROPERTY(vertex, operator);
BOOST_INSTALL_PROPERTY(vertex, inner_graph);
BOOST_INSTALL_PROPERTY(edge, source_port);
BOOST_INSTALL_PROPERTY(edge, target_port);
}  // namespace boost

class DAG {
public:
    /*
     * Types related to the graph representation using the Boost graph library
     */
    using EdgeSourcePort = boost::property<source_port_t, int>;
    using EdgeTargetPort = boost::property<target_port_t, int, EdgeSourcePort>;
    using EdgeProperties = EdgeTargetPort;
    using VertexOperator =
            boost::property<operator_t, std::shared_ptr<DAGOperator>>;
    using VertexInnerDag = boost::property<inner_graph_t, std::shared_ptr<DAG>,
                                           VertexOperator>;
    using VertexProperties = VertexInnerDag;
    // Use vecS for edges because we want to allow parallel edges.
    // Use vecS for vertices because we need internal properties for vertexes.
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS,
                                        boost::bidirectionalS, VertexProperties,
                                        EdgeProperties>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
    using Edge = boost::graph_traits<Graph>::edge_descriptor;

    /*
     * Represents a "flow" between two ports of two operators. Used to return
     * all information related to a flow at once for convenience.
     */
    struct FlowTip {
        DAGOperator *op;
        int port;
    };
    struct Flow {
        FlowTip source;
        FlowTip target;
        Edge e;
    };

private:
    /*
     * Functors used to create iterators
     */
    struct EdgeToFlowFunc {
        explicit EdgeToFlowFunc(const DAG *const dag) : dag_(dag) {}
        auto operator()(const Edge &e) const -> Flow {
            return dag_->to_flow(e);
        }
        const DAG *const dag_;
    };

    struct FilterFlowBySourcePortFunc {
        explicit FilterFlowBySourcePortFunc(const int source_port)
            : source_port_(source_port) {}
        auto operator()(const Flow &f) const -> bool {
            return f.source.port == source_port_;
        }
        const int source_port_;
    };

    struct FilterFlowByTargetPortFunc {
        explicit FilterFlowByTargetPortFunc(const int target_port)
            : target_port_(target_port) {}
        auto operator()(const Flow &f) const -> bool {
            return f.target.port == target_port_;
        }
        const int target_port_;
    };

    struct VertexToOperatorFunc {
        explicit VertexToOperatorFunc(const DAG *const dag) : dag_(dag) {}
        auto operator()(const Vertex &v) const -> DAGOperator * {
            return dag_->to_operator(v);
        }
        const DAG *const dag_;
    };

    struct FilterInputByOperatorFunc {
        explicit FilterInputByOperatorFunc(const DAGOperator *const op)
            : op_(op) {}
        template <typename T>
        auto operator()(const T &it) const -> bool {
            return it.second.op == op_;
        }
        const DAGOperator *const op_;
    };

    struct ExtractFlowTipFunc {
        template <typename T>
        auto operator()(const T &it) const {
            return it.second;
        }
    };

    /*
     * Iterator types (implementation details)
     */
    using EdgeIterator = boost::graph_traits<Graph>::edge_iterator;
    using InEdgeIterator = boost::graph_traits<Graph>::in_edge_iterator;
    using OutEdgeIterator = boost::graph_traits<Graph>::out_edge_iterator;
    using VertexIterator = boost::graph_traits<Graph>::vertex_iterator;

    using EdgeRange = boost::iterator_range<EdgeIterator>;
    using InEdgeRange = boost::iterator_range<InEdgeIterator>;
    using OutEdgeRange = boost::iterator_range<OutEdgeIterator>;
    using VertexRange = boost::iterator_range<VertexIterator>;

public:
    /*
     * Iterator types (public interface)
     */
    using FlowRange = boost::transformed_range<EdgeToFlowFunc, const EdgeRange>;
    using InFlowRange =
            boost::transformed_range<EdgeToFlowFunc, const InEdgeRange>;
    using OutFlowRange =
            boost::transformed_range<EdgeToFlowFunc, const OutEdgeRange>;
    using InFlowByTargetPortRange =
            boost::filtered_range<FilterFlowByTargetPortFunc,
                                  const InFlowRange>;
    using OutFlowBySourcePortRange =
            boost::filtered_range<FilterFlowBySourcePortFunc,
                                  const OutFlowRange>;
    using OperatorRange =
            boost::transformed_range<VertexToOperatorFunc, const VertexRange>;
    using InputRange =
            boost::iterator_range<std::multimap<int, FlowTip>::const_iterator>;
    using InputRangeTips =
            boost::transformed_range<ExtractFlowTipFunc, const InputRange>;
    using InputRangeByOperator =
            const boost::filtered_range<FilterInputByOperatorFunc,
                                        const InputRange>;
    using OutputRange =
            boost::iterator_range<std::map<int, FlowTip>::const_iterator>;
    using OutputRangeByOperator =
            const boost::filtered_range<FilterInputByOperatorFunc,
                                        const OutputRange>;

    /*
     * Normal member functions
     */
private:
    auto AddOperator(DAGOperator *op, const VertexProperties &properties)
            -> Vertex;
    auto AddOperator(DAGOperator *op, size_t id,
                     const VertexProperties &properties) -> Vertex;

public:
    auto AddOperator(DAGOperator *op, DAG *inner_dag = nullptr) -> Vertex;
    auto AddOperator(DAGOperator *op, size_t id, DAG *inner_dag = nullptr)
            -> Vertex;
    auto ReplaceOperator(const DAGOperator *old_op, DAGOperator *new_op)
            -> Vertex;
    void RemoveOperator(const DAGOperator *op);
    void MoveOperator(DAG *other_dag, DAGOperator *op);

    auto AddFlow(const DAGOperator *source, const DAGOperator *target,
                 int target_port = 0) -> Edge;
    auto AddFlow(const DAGOperator *source, int source_port,
                 const DAGOperator *target, int target_port = 0) -> Edge;
    auto AddFlow(const DAGOperator *source, const FlowTip &target) -> Edge;
    auto AddFlow(const DAGOperator *source, int source_port,
                 const FlowTip &target) -> Edge;
    auto AddFlow(const FlowTip &source, const DAGOperator *target,
                 int target_port = 0) -> Edge;
    auto AddFlow(const FlowTip &source, const FlowTip &target) -> Edge;
    void RemoveFlow(const Flow &f) { RemoveFlow(f.e); }
    void RemoveFlow(const Edge &e);

    void Clear();

    auto HasCycle() const -> bool;
    auto IsTree() const -> bool;

    /*
     * Getter/setter type functions
     */
    auto graph() const -> const Graph & { return graph_; }

    auto operators() const -> OperatorRange;
    auto flows() const -> FlowRange;

    auto contains(const DAGOperator *op) const -> bool;

    auto to_operator(const Vertex &v) const -> DAGOperator *;
    auto to_flow(const Edge &e) const -> Flow;
    auto to_vertex(const DAGOperator *op) const -> Vertex;
    auto to_edge(const Flow &f) const -> Edge { return f.e; }

    auto in_flows(const DAGOperator *op) const -> InFlowRange;
    auto out_flows(const DAGOperator *op) const -> OutFlowRange;
    auto in_flows(const DAGOperator *op, int target_port) const
            -> InFlowByTargetPortRange;
    auto out_flows(const DAGOperator *op, int source_port) const
            -> OutFlowBySourcePortRange;

    auto in_flow(const DAGOperator *op) const -> Flow;
    auto in_flow(const DAGOperator *op, int target_port) const -> Flow;
    auto out_flow(const DAGOperator *op) const -> Flow;
    auto out_flow(const DAGOperator *op, int source_port) const -> Flow;

    auto in_degree(const DAGOperator *op) const -> size_t;
    auto in_degree(const DAGOperator *op, int target_port) const -> size_t;
    auto out_degree(const DAGOperator *op) const -> size_t;
    auto out_degree(const DAGOperator *op, int source_port) const -> size_t;

    auto predecessor(const DAGOperator *op) const -> DAGOperator *;
    auto predecessor(const DAGOperator *op, int target_port) const
            -> DAGOperator *;
    auto successor(const DAGOperator *op) const -> DAGOperator *;
    auto successor(const DAGOperator *op, int source_port) const
            -> DAGOperator *;

    auto has_inner_dag(const DAGOperator *op) const -> bool;
    auto inner_dag(const DAGOperator *op) const -> DAG *;
    void set_inner_dag(const DAGOperator *op, DAG *inner_dag);
    void remove_inner_dag(const DAGOperator *op);

    auto in_degree() const -> size_t;
    auto num_in_ports() const -> size_t;
    auto inputs() const -> InputRange;
    auto inputs(int dag_input_port) const -> InputRangeTips;
    auto inputs(const DAGOperator *op) const -> InputRangeByOperator;
    auto input() const -> FlowTip;
    auto input(int dag_input_port) const -> FlowTip;
    auto input(const DAGOperator *op) const -> std::pair<int, FlowTip>;
    auto input_port(const DAGOperator *op) const -> int;
    void set_input(const FlowTip &input);
    void set_input(DAGOperator *op, int operator_input_port = 0);
    void set_input(int dag_input_port, const FlowTip &input);
    void set_input(int dag_input_port, DAGOperator *op,
                   int operator_input_port = 0);
    void add_input(int dag_input_port, const FlowTip &input);
    void add_input(int dag_input_port, DAGOperator *op,
                   int operator_input_port = 0);
    void reset_inputs();

    auto out_degree() const -> size_t;
    auto num_out_ports() const -> size_t;
    auto outputs() const -> OutputRange;
    auto outputs(const DAGOperator *op) const -> OutputRangeByOperator;
    auto output() const -> FlowTip;
    auto output(int dag_output_port) const -> FlowTip;
    void set_output(const FlowTip &output);
    void set_output(DAGOperator *op, int operator_output_port = 0);
    void set_output(int dag_output_port, const FlowTip &output);
    void set_output(int dag_output_port, DAGOperator *op,
                    int operator_output_port = 0);
    void reset_outputs();

private:
    auto last_operator_id() const -> size_t;

    Graph graph_;
    std::set<size_t> operator_ids_;
    std::multimap<int, FlowTip> inputs_;
    std::map<int, FlowTip> outputs_;
};

namespace nlohmann {

template <>
struct adl_serializer<std::unique_ptr<DAG>> {
    static auto from_json(const nlohmann::json &json) -> std::unique_ptr<DAG>;
    // NOLINTNEXTLINE(google-runtime-references)
    static void to_json(nlohmann::json &json, const std::unique_ptr<DAG> &dag);
};

}  // namespace nlohmann

// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, DAG *dag);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const DAG *dag);
// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const std::unique_ptr<const DAG> &dag);

auto ParseDag(std::istream *istream) -> DAG *;
auto ParseDag(const std::string &dagstr) -> DAG *;

#endif  // DAG_DAG_HPP
