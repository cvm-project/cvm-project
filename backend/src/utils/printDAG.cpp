//
// Created by sabir on 27.08.17.
//

#include "printDAG.h"

#include <cstdio>

#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/irange.hpp>

#include <graphviz/gvc.h>

#include "dag/DAG.h"
#include "dag/DAGOperator.h"
#include "utils/utils.h"

using PortInfoMap = std::unordered_map<
        const DAGOperator *,
        std::unordered_multimap<int, std::tuple<Agnode_t *, std::string>>>;

std::string node_name(const std::string &graph_name,
                      const DAGOperator *const op) {
    return (boost::format("%1%%2%_%3%") % graph_name % op->name() % op->id)
            .str();
}

std::string head_port(const DAG::Flow &flow) {
    if (flow.target.op->num_in_ports() > 1) {
        return (boost::format("in%1%:n") % flow.target.port).str();
    }
    return "n";
}

std::string tail_port(const DAG::Flow &flow) {
    if (flow.source.op->num_out_ports() > 1) {
        return (boost::format("out%1%:s") % flow.source.port).str();
    }
    return "s";
}

char *to_char_ptr(const std::string &s) {
    return const_cast<char *>(s.c_str());
}

std::string short_property_label(const dag::collection::FieldProperty prop) {
    const static std::unordered_map<dag::collection::FieldProperty, std::string,
                                    std::hash<int>>
            property_labels = {
                    {dag::collection::FL_UNIQUE, "u"},  //
                    {dag::collection::FL_SORTED, "s"},  //
                    {dag::collection::FL_GROUPED, "g"}  //
            };
    return property_labels.at(prop);
}

void buildDOT(const DAG *const dag, Agraph_t *g,
              const std::string &graph_name = "") {
    // Store which (GV) node and port label an edge should end at for a given
    // operator and port
    PortInfoMap input_port_labels;
    PortInfoMap output_port_labels;

    // Compute nodes from operators
    for (const auto &op : dag->operators()) {
        if (!dag->has_inner_dag(op)) {
            // Normal case: regular operator --> node
            const std::string op_name = node_name(graph_name, op);

            // Fields
            std::vector<std::string> fields;
            if (op->tuple) {
                for (const auto &f : op->tuple->fields) {
                    const std::string column_name =
                            f->attribute_id() != nullptr
                                    ? f->attribute_id()->name()
                                    : "?";

                    std::vector<std::string> property_flags;
                    boost::transform(
                            boost::make_iterator_range(f->properties()),
                            std::back_inserter(property_flags),
                            short_property_label);
                    const std::string property_label =
                            property_flags.empty()
                                    ? ""
                                    : (boost::format(" (%s)") %
                                       boost::join(property_flags, ""))
                                              .str();

                    fields.emplace_back(column_name + property_label);
                }
            }

            // Read set
            std::vector<std::string> read_set;
            std::transform(op->read_set.begin(), op->read_set.end(),
                           std::back_inserter(read_set),
                           [](const auto &c) { return c->name(); });

            // Write set
            std::vector<std::string> write_set;
            std::transform(op->write_set.begin(), op->write_set.end(),
                           std::back_inserter(write_set),
                           [](const auto &c) { return c->name(); });

            // Input ports
            const auto input_ports =
                    boost::irange(size_t{0}, op->num_in_ports()) |
                    boost::adaptors::transformed([](const auto &i) {
                        const std::string s = std::to_string(i);
                        return (boost::format("<in%1%> %1%") % s).str();
                    });
            const auto input_ports_label =
                    (boost::format("{%s} | ") % boost::join(input_ports, "| "))
                            .str();
            const auto input_port_label = (op->num_in_ports() > 1)
                                                  ? input_ports_label
                                                  : std::string();

            // Output ports
            const auto output_ports =
                    boost::irange(size_t{0}, op->num_out_ports()) |
                    boost::adaptors::transformed([](const auto &i) {
                        const std::string s = std::to_string(i);
                        return (boost::format("<out%1%> %1%") % s).str();
                    });
            const auto output_ports_label =
                    (boost::format(" | {%s}") % boost::join(output_ports, "| "))
                            .str();
            const auto output_port_label = (op->num_out_ports() > 1)
                                                   ? output_ports_label
                                                   : std::string();

            // Compute node label
            const std::string node_label =
                    (boost::format("{%s <header>%s (%s%i) | <fields> fields: "
                                   "%s&#92;nread: %s&#92;nwrite: %s&#92;n%s}") %
                     input_port_label % op->name() % graph_name % op->id %  //
                     boost::join(fields, ", ") %                            //
                     boost::join(read_set, ", ") %                          //
                     boost::join(write_set, ", ") %                         //
                     output_port_label)
                            .str();

            // Create node
            Agnode_t *node = agnode(g, to_char_ptr(op_name), 1);
            agsafeset(node, "shape", "record", "record");
            agsafeset(node, "label", to_char_ptr(node_label), "");

            // Store node and port info
            for (const auto f : dag->in_flows(op)) {
                input_port_labels[op].emplace(
                        f.target.port, std::make_tuple(node, head_port(f)));
            }
            for (const auto f : dag->out_flows(op)) {
                output_port_labels[op].emplace(
                        f.source.port, std::make_tuple(node, tail_port(f)));
            }
        } else {
            // Special case: nested operator --> cluster
            assert(dag->has_inner_dag(op));
            const std::string op_name = node_name(graph_name, op);

            // Create cluster
            auto const cluster_name = "cluster_" + op_name;
            auto const cluster_label = (boost::format("%s (%s%i)") %
                                        op->name() % graph_name % op->id)
                                               .str();
            Agraph_t *subg = agsubg(g, to_char_ptr(cluster_name), 1);
            agsafeset(subg, "label", to_char_ptr(cluster_label), "");
            agsafeset(subg, "style", "filled", "");

            // Create inner graph
            const auto inner_dag = dag->inner_dag(op);
            assert(dag->inner_dag(op) != nullptr);
            const std::string inner_graph_name =
                    graph_name + std::to_string(op->id) + "_";
            buildDOT(inner_dag, subg, inner_graph_name);

            // Store port info of input ports
            for (const auto f : dag->in_flows(op)) {
                // Incoming edges should point to the corresponding parameter
                // lookup operator
                for (const auto input : inner_dag->inputs(f.target.port)) {
                    auto const target_op = input.op;
                    auto const node = agnode(
                            g,
                            to_char_ptr(node_name(inner_graph_name, target_op)),
                            0);
                    assert(node != nullptr);
                    input_port_labels[op].emplace(f.target.port,
                                                  std::make_tuple(node, "n"));
                }
            }

            // Store port info of output ports
            for (const auto f : dag->out_flows(op)) {
                // Outgoing edges come from the sink
                assert(f.source.port == 0);
                auto const node =
                        agnode(g,
                               to_char_ptr(node_name(inner_graph_name,
                                                     inner_dag->output().op)),
                               0);
                assert(node != nullptr);
                output_port_labels[op].emplace(f.source.port,
                                               std::make_tuple(node, "s"));
            }
        }
    }

    // Compute edges from flows
    for (const auto &f : dag->flows()) {
        // Lookup port infos for source and target
        const auto source_infos =
                output_port_labels.at(f.source.op).equal_range(f.source.port);
        const auto target_infos =
                input_port_labels.at(f.target.op).equal_range(f.target.port);

        const auto source_info_range = boost::make_iterator_range(source_infos);
        const auto target_info_range = boost::make_iterator_range(target_infos);

        // Add an edge for each combination
        for (auto const &source_info : source_info_range) {
            for (auto const &target_info : target_info_range) {
                const auto source_node = get<0>(source_info.second);
                const auto target_node = get<0>(target_info.second);

                const auto &source_port_label = get<1>(source_info.second);
                const auto &target_port_label = get<1>(target_info.second);

                // Create edge
                const auto edge_name = std::to_string(f.source.port) + "-" +
                                       std::to_string(f.target.port);
                Agedge_t *const edge = agedge(g, source_node, target_node,
                                              to_char_ptr(edge_name), 1);

                // Set edge ports
                agsafeset(edge, "headport", to_char_ptr(target_port_label), "");
                agsafeset(edge, "tailport", to_char_ptr(source_port_label), "");
            }
        }
    }
}

const char *ToDotCharPtr(const DAG *dag) {
    char *buffer_ptr;
    size_t buffer_size;
    FILE *const outfile = open_memstream(&buffer_ptr, &buffer_size);

    ToDotFile(dag, outfile);
    fclose(outfile);

    return buffer_ptr;
}

std::string ToDotString(const DAG *dag) {
    const char *const buffer_ptr = ToDotCharPtr(dag);
    const std::string ret(buffer_ptr);
    free(const_cast<char *>(buffer_ptr));
    return ret;
}

void ToDotFile(const DAG *dag, FILE *outfile) {
    Agraph_t *const g = agopen("g", Agdirected, &AgDefaultDisc);
    agsafeset(g, "compound", "true", "true");

    buildDOT(dag, g);

    GVC_t *const gvc = gvContext();
    gvLayout(gvc, g, "dot");
    gvRender(gvc, g, "dot", outfile);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
}

void ToDotStream(const DAG *dag, std::ostream *outstream) {
    const char *const buffer_ptr = ToDotCharPtr(dag);
    *outstream << buffer_ptr;
    free(const_cast<char *>(buffer_ptr));
}
