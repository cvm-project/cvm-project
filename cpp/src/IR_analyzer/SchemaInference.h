//
// Created by sabir on 26.08.17.
//

#ifndef CPP_SCHEMAINFERENCE_H
#define CPP_SCHEMAINFERENCE_H

#include "utils/DAGVisitor.h"
#include "IR_analyzer/LLVMParser.h"

/**
 * assign schema for every operator
 * compute read, write and dead sets
 */
class SchemaInference : DAGVisitor {
public:
    void start_visit(DAG *dag) {
        dag->sink->accept(*this);
    }

    void visit(DAGCollection *op) {
        DEBUG_PRINT("visiting collection");
        visitPredecessors(op);
        //for every output produce a new column
        for (size_t i = 0; i < op->fields.size(); i++) {
            Column *c = Column::makeColumn();
            c->addField(&(op->fields[i]));
            //add all cols to the write set
            op->write_set.insert(c);
        }
    }

    void visit(DAGRange *op) {
        DEBUG_PRINT("visiting range");
        visitPredecessors(op);

        //for every output produce a new column
        for (size_t i = 0; i < op->fields.size(); i++) {
            Column *c = Column::makeColumn();
            c->addField(&(op->fields[i]));
            //add all cols to the write set
            op->write_set.insert(c);
        }
    }

    void visit(DAGFilter *op) {
        DEBUG_PRINT("visiting filter");
        visitPredecessors(op);

        LLVMParser parser(op->llvm_ir);
        //copy output columns from the predecessor
        for (size_t i = 0; i < op->fields.size(); i++) {
            Column *col = op->predecessors[0]->fields[i].column;
            op->fields[i].column = col;

            if (parser.is_argument_read(i)) {
                op->read_set.insert(col);
            }
        }
    }

    void visit(DAGJoin *op) {
        DEBUG_PRINT("visiting join");
        visitPredecessors(op);

        DAGOperator *left = op->predecessors[0];
        DAGOperator *right = op->predecessors[1];
        Column *keyCol = left->fields[0].column;
        op->fields[0].column = keyCol;

        //we can now assume that the key column is the same on left and right
        //remap right inputs key column to left
        *(right->fields[0].column) = *keyCol;
        keyCol->addFields(right->fields[0].column->getFields());

        for (size_t i = 1; i < op->fields.size(); i++) {
            if (i < left->fields.size()) {
                op->fields[i].column = left->fields[i].column;
            }
            else {
                op->fields[i].column = right->fields[i - left->fields.size() + 1].column;
            }
        }
    }

    void visit(DAGMap *op) {
        DEBUG_PRINT("visiting map");
        visitPredecessors(op);
        LLVMParser parser(op->llvm_ir);

        size_t c = 0;
        for (auto arg : op->predecessors[0]->fields) {
            bool used = 0;
            for (auto pos : parser.get_output_positions(c)) {
                //every output in the list now has the same column type as this input
                arg.column->addField(&(op->fields[pos]));
                used = 1;
            }
            if (parser.is_argument_read(c)) {
                op->read_set.insert(arg.column);
                used = 1;
            }

            //all the inputs which are not in the read set and are not repeated are dead
            if (!used) {
                op->dead_set.insert(arg);
            }
            c++;
        }
        //all the outputs which do not have the column set yet, are written by the map
        size_t size = op->fields.size();
        for (size_t pos = 0; pos < size; pos++) {
            if (op->fields[pos].column == NULL) {
                Column *c = Column::makeColumn();
                c->addField(&(op->fields[pos]));
                op->write_set.insert(c);
            }
        }
    }

    void visit(DAGReduce *op) {
        DEBUG_PRINT("visiting reduce");
        visitPredecessors(op);
    }

    void visit(DAGReduceByKey *op) {
        DEBUG_PRINT("visiting reduce_by_key");
        visitPredecessors(op);

        //keep the key column
        DAGOperator *pred = op->predecessors[0];
        op->fields[0].column = pred->fields[0].column;

        //generate a new column for every output
        for (size_t i = 1; i < op->fields.size(); i++) {
            Column *c = Column::makeColumn();
            c->addField(&(op->fields[i]));
            //add all cols to the write set
            op->write_set.insert(c);
        }
    }
};


#endif //CPP_SCHEMAINFERENCE_H
