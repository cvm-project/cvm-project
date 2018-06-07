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

std::string node_name(const DAGOperator *const op) {
    return op->name() + "_" + std::to_string(op->id);
}

std::string head_port(const DAG::Flow &flow) {
    if (flow.target->num_in_ports() > 1) {
        return std::to_string(flow.target_port) + ":n";
    }
    return "n";
}

char *to_char_ptr(const std::string &s) {
    return const_cast<char *>(s.c_str());
}

std::string short_property_label(const FieldProperty prop) {
    const static std::unordered_map<FieldProperty, std::string, std::hash<int>>
            property_labels = {
                    {FL_UNIQUE, "u"},  //
                    {FL_SORTED, "s"},  //
                    {FL_GROUPED, "g"}  //
            };
    return property_labels.at(prop);
}

void buildDOT(const DAG *const dag, Agraph_t *g) {
    // Compute nodes from operators
    std::unordered_map<const DAGOperator *, Agnode_t *> nodes;
    for (const auto &op : dag->operators()) {
        const std::string op_name = node_name(op);

        // Fields
        std::vector<std::string> fields;
        for (const auto &f : op->fields) {
            const std::string column_name =
                    f.column != nullptr ? f.column->get_name() : "?";

            std::vector<std::string> property_flags;
            boost::transform(boost::make_iterator_range(*f.properties),
                             std::back_inserter(property_flags),
                             short_property_label);
            const std::string property_label =
                    property_flags.empty() ? ""
                                           : (boost::format(" (%s)") %
                                              boost::join(property_flags, ""))
                                                     .str();

            fields.emplace_back(column_name + property_label);
        }

        // Read set
        std::vector<std::string> read_set;
        std::transform(op->read_set.begin(), op->read_set.end(),
                       std::back_inserter(read_set),
                       [](const auto &c) { return c->get_name(); });

        // Write set
        std::vector<std::string> write_set;
        std::transform(op->write_set.begin(), op->write_set.end(),
                       std::back_inserter(write_set),
                       [](const auto &c) { return c->get_name(); });

        // Input ports
        const auto input_ports =
                boost::irange(size_t{0}, op->num_in_ports()) |
                boost::adaptors::transformed([](const auto &i) {
                    const std::string s = std::to_string(i);
                    return (boost::format("<%1%> %1%") % s).str();
                });
        const auto input_ports_label =
                (boost::format("{%s} | ") % boost::join(input_ports, "| "))
                        .str();
        const auto input_port_label =
                (op->num_in_ports() > 1) ? input_ports_label : std::string();

        // Compute node label
        const std::string node_label =
                (boost::format("{%s <header>%s | <fields> fields: "
                               "%s&#92;nread: %s&#92;nwrite: %s&#92;n}") %
                 input_port_label % op_name %   //
                 boost::join(fields, ", ") %    //
                 boost::join(read_set, ", ") %  //
                 boost::join(write_set, ", "))
                        .str();

        // Create node
        Agnode_t *node = agnode(g, to_char_ptr(op_name), 1);
        agsafeset(node, "shape", "record", "record");
        agsafeset(node, "label", to_char_ptr(node_label), "");
        nodes.emplace(op, node);
    }

    // Compute edges from flows
    for (const auto &f : dag->flows()) {
        const auto &source_node = nodes.at(f.source);
        const auto &target_node = nodes.at(f.target);
        Agedge_t *const edge = agedge(g, source_node, target_node, "", 1);

        agsafeset(edge, "headport", to_char_ptr(head_port(f)), "");
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
