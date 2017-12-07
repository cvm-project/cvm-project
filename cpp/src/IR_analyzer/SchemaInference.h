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
        DEBUG_PRINT("schema inference visiting collection");
        visitPredecessors(op);
        //for every output produce a new column
        for (size_t i = 0; i < op->fields.size(); i++) {
            Column *c = Column::makeColumn();
            c->addField(&(op->fields[i]));
            //add all cols to the write set
            op->write_set.insert(c);
        }
        if (op->add_index) {
            op->fields[0].properties->insert(FL_UNIQUE);
            op->fields[0].properties->insert(FL_SORTED);
        }
    }

    void visit(DAGRange *op) {
        DEBUG_PRINT("schema inference visiting range");
        visitPredecessors(op);

        //for every output produce a new column
        for (size_t i = 0; i < op->fields.size(); i++) {
            auto field = &(op->fields[i]);
            Column *c = Column::makeColumn();
            field->properties->insert(FL_UNIQUE);
            field->properties->insert(FL_SORTED);
            c->addField(field);
            //add all cols to the write set
            op->write_set.insert(c);
        }
    }

    void visit(DAGFilter *op) {
        DEBUG_PRINT("schema inference visiting filter");
        visitPredecessors(op);

        LLVMParser parser(op->llvm_ir);
        //copy output columns and fields from the predecessor
        for (size_t i = 0; i < op->fields.size(); i++) {
            Column *col = op->predecessors[0]->fields[i].column;
            op->fields[i].column = col;
            op->fields[i].properties = op->predecessors[0]->fields[i].properties;

            if (parser.is_argument_read(i)) {
                op->read_set.insert(col);
            }
        }
    }

    void visit(DAGCartesian *op) {
        DEBUG_PRINT("schema inference cartesian");
        visitPredecessors(op);

        //copy output columns and fields from the predecessors
        for (size_t i = 0; i < op->predecessors[0]->fields.size(); i++) {
            Column *col = op->predecessors[0]->fields[i].column;
            op->fields[i].column = col;
            auto field = &(op->fields[i]);
            field->properties = op->predecessors[0]->fields[i].properties;
            //TODO could still keep the props if the other side has size 1
            if (op->stream_right) {
                //strip away properties
                field->properties->erase(FL_SORTED);
                field->properties->erase(FL_UNIQUE);
                field->properties->erase(FL_GROUPED);
            }
            else {
                field->properties->erase(FL_UNIQUE);
            }
        }

        for (size_t i = 0; i < op->predecessors[1]->fields.size(); i++) {
            Column *col = op->predecessors[1]->fields[i].column;
            auto field = &(op->fields[i + op->predecessors[0]->fields.size()]);
            field->column = col;
            field->properties = op->predecessors[1]->fields[i].properties;
            if (!op->stream_right) {
                field->properties->erase(FL_SORTED);
                field->properties->erase(FL_UNIQUE);
                field->properties->erase(FL_GROUPED);
            }
            else {
                field->properties->erase(FL_UNIQUE);
            }
        }
    }

    void visit(DAGJoin *op) {
        DEBUG_PRINT("schema inference visiting join");
        visitPredecessors(op);
        DAGOperator *builtPred = op->predecessors[0];
        DAGOperator *streamPred = op->predecessors[1];
        if (!op->stream_right) {
            builtPred = op->predecessors[1];
            streamPred = op->predecessors[0];
        }
        Column *keyCol = builtPred->fields[0].column;
        auto keyField = &(builtPred->fields[0]);
        op->fields[0].column = keyCol;

        //determine if the both sides keys were unique
        auto streamPredKeyField = &(streamPred->fields[0]);
        bool unique_key = keyField->properties->find(FL_UNIQUE) != keyField->properties->end() &&
                          streamPredKeyField->properties->find(FL_UNIQUE) != streamPredKeyField->properties->end();

        if (!unique_key) {
            //with current implementation the other properties should be preserved
            keyField->properties->erase(FL_UNIQUE);
        }
        //we can now assume that the key column is the same on the left and right
        //remap streamPred inputs key column to builtPred
        auto streamPredKeyCol = streamPredKeyField->column;
        *(streamPredKeyCol) = *keyCol;
        keyCol->addFields(streamPred->fields[0].column->getFields());

        auto left = builtPred;
        auto right = streamPred;
        if (!op->stream_right) {
            left = streamPred;
            right = builtPred;
        }
        for (size_t i = 1; i < op->fields.size(); i++) {
            if (i < left->fields.size()) {
                auto predCol = left->fields[i].column;
                op->fields[i].column = predCol;
                if (op->stream_right) {
                    //this is the build relation
                    //TODO
                }
            }
            else {
                auto predCol = right->fields[i - left->fields.size() + 1].column;
                op->fields[i].column = predCol;
                //TODO column properties
            }
        }
    }

    void visit(DAGMap *op) {
        DEBUG_PRINT("schema inference visiting map");
        visitPredecessors(op);
        LLVMParser parser(op->llvm_ir);

        size_t c = 0;
        for (auto arg : op->predecessors[0]->fields) {
            bool used = 0;
            for (auto pos : parser.get_output_positions(c)) {
                //every output in the list now has the same column type as this input
                arg.column->addField(&(op->fields[pos]));

                *(op->fields[pos].properties) = *(arg.properties);
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

    void visit(DAGReduceByKey *op) {
        DEBUG_PRINT("schema inference visiting reduce_by_key");
        visitPredecessors(op);

        //keep the key column
        DAGOperator *pred = op->predecessors[0];
        auto firstField = &(op->fields[0]);

        firstField->column = pred->fields[0].column;
        //add key to the read set
        op->read_set.insert(firstField->column);
        //alias the properties
        firstField->properties = pred->fields[0].properties;

        LLVMParser parser(op->llvm_ir);

        for (size_t i = 1; i < op->fields.size(); i++) {
            //TODO
            //check which of the columns are read from the second input

            size_t arg_pos = i + op->fields.size() - 2;
            auto arg = op->predecessors[0]->fields[i];
            if (parser.is_argument_read(arg_pos)) {
                op->read_set.insert(arg.column);
            }
            else {
                //add to the dead set
                op->dead_set.insert(arg);
            }

            //generate a new column for every output
            Column *c = Column::makeColumn();
            c->addField(&(op->fields[i]));
            //add all cols to the write set
            op->write_set.insert(c);
        }
    }
};


#endif //CPP_SCHEMAINFERENCE_H