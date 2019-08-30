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
        Flow operator()(const Edge &e) const { return dag_->to_flow(e); }
        const DAG *const dag_;
    };

    struct FilterFlowBySourcePortFunc {
        explicit FilterFlowBySourcePortFunc(const int source_port)
            : source_port_(source_port) {}
        bool operator()(const Flow &f) const {
            return f.source.port == source_port_;
        }
        const int source_port_;
    };

    struct FilterFlowByTargetPortFunc {
        explicit FilterFlowByTargetPortFunc(const int target_port)
            : target_port_(target_port) {}
        bool operator()(const Flow &f) const {
            return f.target.port == target_port_;
        }
        const int target_port_;
    };

    struct VertexToOperatorFunc {
        explicit VertexToOperatorFunc(const DAG *const dag) : dag_(dag) {}
        DAGOperator *operator()(const Vertex &v) const {
            return dag_->to_operator(v);
        }
        const DAG *const dag_;
    };

    struct FilterInputByOperatorFunc {
        explicit FilterInputByOperatorFunc(const DAGOperator *const op)
            : op_(op) {}
        template <typename T>
        bool operator()(const T &it) const {
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
    Vertex AddOperator(DAGOperator *op, const VertexProperties &properties);
    Vertex AddOperator(DAGOperator *op, size_t id,
                       const VertexProperties &properties);

public:
    Vertex AddOperator(DAGOperator *op, DAG *inner_dag = nullptr);
    Vertex AddOperator(DAGOperator *op, size_t id, DAG *inner_dag = nullptr);
    void RemoveOperator(const DAGOperator *op);
    void MoveOperator(DAG *other_dag, DAGOperator *op);

    Edge AddFlow(const DAGOperator *source, const DAGOperator *target,
                 int target_port = 0);
    Edge AddFlow(const DAGOperator *source, int source_port,
                 const DAGOperator *target, int target_port = 0);
    Edge AddFlow(const DAGOperator *source, const FlowTip &target);
    Edge AddFlow(const DAGOperator *source, int source_port,
                 const FlowTip &target);
    Edge AddFlow(const FlowTip &source, const DAGOperator *target,
                 int target_port = 0);
    Edge AddFlow(const FlowTip &source, const FlowTip &target);
    void RemoveFlow(const Flow &f) { RemoveFlow(f.e); }
    void RemoveFlow(const Edge &e);

    void Clear();

    bool HasCycle() const;
    bool IsTree() const;

    /*
     * Getter/setter type functions
     */
    const Graph &graph() const { return graph_; }

    OperatorRange operators() const;
    FlowRange flows() const;

    bool contains(const DAGOperator *op) const;

    DAGOperator *to_operator(const Vertex &v) const;
    Flow to_flow(const Edge &e) const;
    Vertex to_vertex(const DAGOperator *op) const;
    Edge to_edge(const Flow &f) const { return f.e; }

    InFlowRange in_flows(const DAGOperator *op) const;
    OutFlowRange out_flows(const DAGOperator *op) const;
    InFlowByTargetPortRange in_flows(const DAGOperator *op,
                                     int target_port) const;
    OutFlowBySourcePortRange out_flows(const DAGOperator *op,
                                       int source_port) const;

    Flow in_flow(const DAGOperator *op) const;
    Flow in_flow(const DAGOperator *op, int target_port) const;
    Flow out_flow(const DAGOperator *op) const;
    Flow out_flow(const DAGOperator *op, int source_port) const;

    size_t in_degree(const DAGOperator *op) const;
    size_t in_degree(const DAGOperator *op, int target_port) const;
    size_t out_degree(const DAGOperator *op) const;
    size_t out_degree(const DAGOperator *op, int source_port) const;

    DAGOperator *predecessor(const DAGOperator *op) const;
    DAGOperator *predecessor(const DAGOperator *op, int target_port) const;
    DAGOperator *successor(const DAGOperator *op) const;
    DAGOperator *successor(const DAGOperator *op, int source_port) const;

    bool has_inner_dag(const DAGOperator *op) const;
    DAG *inner_dag(const DAGOperator *op) const;
    void set_inner_dag(const DAGOperator *op, DAG *inner_dag);
    void remove_inner_dag(const DAGOperator *op);

    size_t in_degree() const;
    size_t num_in_ports() const;
    InputRange inputs() const;
    InputRangeTips inputs(int dag_input_port) const;
    InputRangeByOperator inputs(const DAGOperator *op) const;
    FlowTip input() const;
    FlowTip input(int dag_input_port) const;
    std::pair<int, FlowTip> input(const DAGOperator *op) const;
    int input_port(const DAGOperator *op) const;
    void set_input(const FlowTip &input);
    void set_input(DAGOperator *op, int operator_input_port = 0);
    void set_input(int dag_input_port, const FlowTip &input);
    void set_input(int dag_input_port, DAGOperator *op,
                   int operator_input_port = 0);
    void add_input(int dag_input_port, const FlowTip &input);
    void add_input(int dag_input_port, DAGOperator *op,
                   int operator_input_port = 0);
    void reset_inputs();

    size_t out_degree() const;
    size_t num_out_ports() const;
    OutputRange outputs() const;
    OutputRangeByOperator outputs(const DAGOperator *op) const;
    FlowTip output() const;
    FlowTip output(int dag_output_port) const;
    void set_output(const FlowTip &output);
    void set_output(DAGOperator *op, int operator_output_port = 0);
    void set_output(int dag_output_port, const FlowTip &output);
    void set_output(int dag_output_port, DAGOperator *op,
                    int operator_output_port = 0);
    void reset_outputs();

private:
    size_t last_operator_id() const;

    Graph graph_;
    std::set<size_t> operator_ids_;
    std::multimap<int, FlowTip> inputs_;
    std::map<int, FlowTip> outputs_;
};

namespace nlohmann {

template <>
struct adl_serializer<std::unique_ptr<DAG>> {
    static std::unique_ptr<DAG> from_json(const nlohmann::json &json);
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

DAG *ParseDag(std::istream *istream);
DAG *ParseDag(const std::string &dagstr);

#endif  // DAG_DAG_HPP
