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

#include <graphviz/gvc.h>

#include "dag/DAG.h"
#include "dag/DAGOperator.h"
#include "utils/utils.h"

void buildDOT(const DAG *const dag, Agraph_t *g) {
    // Compute nodes from operators
    std::unordered_map<const DAGOperator *, Agnode_t *> nodes;
    for (const auto &op : dag->operators()) {
        const std::string op_name = op->name() + "_" + std::to_string(op->id);

        // Fields
        std::vector<std::string> fields;
        std::transform(op->fields.begin(), op->fields.end(),
                       std::back_inserter(fields), [](const auto &f) {
                           return f.column != nullptr ? f.column->get_name()
                                                      : "?";
                       });

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

        // Dead set
        std::vector<std::string> dead_set;
        std::transform(op->dead_set.begin(), op->dead_set.end(),
                       std::back_inserter(dead_set), [](const auto &f) {
                           return "o_" + std::to_string(f.position);
                       });

        // Compute node label
        const std::string node_label =
                (boost::format(
                         "%s\n\nfields: %s\nread: %s\nwrite: %s\ndead: %s\n") %
                 op_name %                       //
                 boost::join(fields, ", ") %     //
                 boost::join(read_set, ", ") %   //
                 boost::join(write_set, ", ") %  //
                 boost::join(dead_set, ", "))
                        .str();

        // Create node
        Agnode_t *node = agnode(g, const_cast<char *>(node_label.c_str()), 1);
        agsafeset(node, "shape", "polygon", "polygon");
        nodes.emplace(op, node);
    }

    // Compute edges from flows
    for (const auto &f : dag->flows()) {
        const auto &source_node = nodes.at(f.source);
        const auto &target_node = nodes.at(f.target);
        agedge(g, source_node, target_node, "", 1);
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
