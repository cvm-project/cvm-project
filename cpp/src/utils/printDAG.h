//
// Created by sabir on 27.08.17.
//

#ifndef CPP_PRINTDAG_H
#define CPP_PRINTDAG_H

#include <graphviz/gvc.h>

#include "dag/DAG.h"
#include "utils/utils.h"

Agnode_t *buildDOT(DAGOperator *op, Agraph_t *g) {
    string label = op->get_name();

    string outCols = "columns:  ";
    //    string outputTypes = "output types:  ";
    for (auto c : op->fields) {
        if (c.column != NULL) {
            outCols += c.column->get_name() + ", ";
        }
        //        outputTypes += c.type;
        //        for (auto p : *(c.properties)) {
        //            outputTypes += to_string(p) + " prop, ";
        //        }
    }
    //    outputTypes.pop_back();
    //    outputTypes.pop_back();
    outCols.pop_back();
    outCols.pop_back();
    label += "\n\n";

    string readSet = "read:  ";
    for (auto c : op->read_set) {
        readSet += c->get_name() + ", ";
    }

    readSet.pop_back();
    readSet.pop_back();
    label += "\n" + readSet;

    string writeSet = "write:  ";
    for (auto c : op->write_set) {
        writeSet += c->get_name() + ", ";
    }

    writeSet.pop_back();
    writeSet.pop_back();
    label += "\n" + writeSet;

    string deadVars = "dead:  ";
    for (auto c : op->dead_set) {
        deadVars += "o_" + to_string(c.position) + ", ";
    }

    deadVars.pop_back();
    deadVars.pop_back();
    //    label += "\n" + deadVars;

    Agnode_t *n = agnode(g, const_cast<char *>(op->get_name().c_str()), 1);
    agsafeset(n, "shape", "polygon", "polygon");

    for (auto pred : op->predecessors) {
        Agedge_t *e = agedge(g, buildDOT(pred, g), n, "", 1);
    }
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
    fp = fopen("/tmp/dag.dot", "w");
    gvRender(gvc, g, "dot", fp);
    exec("xdot /tmp/dag.dot");
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
}

#endif  // CPP_PRINTDAG_H
