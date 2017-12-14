//
// Created by sabir on 27.08.17.
//

#include "printDAG.h"

#include <string>

#include <graphviz/gvc.h>

#include "dag/DAG.h"
#include "dag/DAGOperator.h"
#include "utils/utils.h"

Agnode_t *buildDOT(DAGOperator *op, Agraph_t *g) {
    std::string label = op->get_name();

    std::string outCols = "columns:  ";
    //    std::string outputTypes = "output types:  ";
    for (auto c : op->fields) {
        if (c.column != nullptr) {
            outCols += c.column->get_name() + ", ";
        }
        //        outputTypes += c.type;
        //        for (auto p : *(c.properties)) {
        //            outputTypes += std::to_string(p) + " prop, ";
        //        }
    }
    //    outputTypes.pop_back();
    //    outputTypes.pop_back();
    outCols.pop_back();
    outCols.pop_back();
    label += "\n\n";

    std::string readSet = "read:  ";
    for (auto c : op->read_set) {
        readSet += c->get_name() + ", ";
    }

    readSet.pop_back();
    readSet.pop_back();
    label += "\n" + readSet;

    std::string writeSet = "write:  ";
    for (auto c : op->write_set) {
        writeSet += c->get_name() + ", ";
    }

    writeSet.pop_back();
    writeSet.pop_back();
    label += "\n" + writeSet;

    std::string deadVars = "dead:  ";
    for (auto c : op->dead_set) {
        deadVars += "o_" + std::to_string(c.position) + ", ";
    }

    deadVars.pop_back();
    deadVars.pop_back();
    //    label += "\n" + deadVars;

    Agnode_t *n = agnode(g, const_cast<char *>(op->get_name().c_str()), 1);
    agsafeset(n, "shape", "polygon", "polygon");

    for (auto pred : op->predecessors) {
        agedge(g, buildDOT(pred, g), n, "", 1);
    }

    (void)label;
    return n;
}

void printDAG(DAG *dag) {
    Agraph_t *g;
    GVC_t *gvc;
    g = agopen("g", Agdirected, &AgDefaultDisc);
    buildDOT(dag->sink, g);
    gvc = gvContext();
    gvLayout(gvc, g, "dot");
    FILE *fp;
    fp = fopen("/tmp/dag.dot", "we");
    gvRender(gvc, g, "dot", fp);
    exec("xdot /tmp/dag.dot");
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
}
