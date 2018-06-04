//
// Created by sabir on 26.08.17.
//

#include "SchemaInference.h"

#include <cassert>

#include "IR_analyzer/LLVMParser.h"
#include "dag/DAG.h"
#include "dag/DAGOperators.h"
#include "dag/TupleField.h"
#include "utils/DAGVisitor.h"
#include "utils/debug_print.h"

void SchemaInference::Run() {
    this->StartVisit();

    for (auto const op : dag()->operators()) {
        for (auto const &f : op->fields) {
            assert(f.column != nullptr);
        }
    }
}

void SchemaInference::visit(DAGCollection *op) {
    DEBUG_PRINT("schema inference visiting collection");
    // for every output produce a new column
    for (size_t i = 0; i < op->fields.size(); i++) {
        Column *c = Column::makeColumn();
        c->addField(&(op->fields[i]));
        // add all cols to the write set
        op->write_set.insert(c);
    }
}

void SchemaInference::visit(DAGRange *op) {
    DEBUG_PRINT("schema inference visiting range");

    // for every output produce a new column
    for (size_t i = 0; i < op->fields.size(); i++) {
        auto field = &(op->fields[i]);
        Column *c = Column::makeColumn();
        c->addField(field);
        // add all cols to the write set
        op->write_set.insert(c);
    }
}

void SchemaInference::visit(DAGFilter *op) {
    DEBUG_PRINT("schema inference visiting filter");

    LLVMParser parser(op->llvm_ir);
    // copy output columns and fields from the predecessor
    for (size_t i = 0; i < op->fields.size(); i++) {
        Column *col = dag()->predecessor(op)->fields[i].column;
        op->fields[i].column = col;

        if (parser.is_argument_read(i)) {
            op->read_set.insert(col);
        }
    }
}

void SchemaInference::visit(DAGCartesian *op) {
    DEBUG_PRINT("schema inference cartesian");

    // copy output columns and fields from the predecessors
    for (size_t i = 0; i < dag()->predecessor(op, 0)->fields.size(); i++) {
        Column *col = dag()->predecessor(op, 0)->fields[i].column;
        op->fields[i].column = col;
    }

    for (size_t i = 0; i < dag()->predecessor(op, 1)->fields.size(); i++) {
        Column *col = dag()->predecessor(op, 1)->fields[i].column;
        auto field =
                &(op->fields[i + dag()->predecessor(op, 0)->fields.size()]);
        field->column = col;
    }
}

void SchemaInference::visit(DAGJoin *op) {
    DEBUG_PRINT("schema inference visiting join");

    auto left = dag()->predecessor(op, 0);
    auto right = dag()->predecessor(op, 1);

    // remap left input's key column to right input's one
    *(right->fields[0].column) = *(left->fields[0].column);
    left->fields[0].column->addFields(right->fields[0].column->getFields());

    for (size_t i = 0; i < op->fields.size(); i++) {
        if (i < left->fields.size()) {
            auto predCol = left->fields[i].column;
            op->fields[i].column = predCol;
        } else {
            auto predCol = right->fields[i - left->fields.size() + 1].column;
            op->fields[i].column = predCol;
        }
    }
}

void SchemaInference::visit(DAGMap *op) {
    DEBUG_PRINT("schema inference visiting map");
    LLVMParser parser(op->llvm_ir);

    size_t c = 0;
    for (auto arg : dag()->predecessor(op)->fields) {
        bool used = false;
        for (auto pos : parser.get_output_positions(c)) {
            // every output in the list now has the same column type as this
            // input
            arg.column->addField(&(op->fields[pos]));

            used = true;
        }
        if (parser.is_argument_read(c)) {
            op->read_set.insert(arg.column);
            used = true;
        }

        // all the inputs which are not in the read set and are not repeated
        // are dead
        if (!used) {
            op->dead_set.insert(arg);
        }
        c++;
    }
    // all the outputs which do not have the column set yet, are written by
    // the map
    size_t size = op->fields.size();
    for (size_t pos = 0; pos < size; pos++) {
        if (op->fields[pos].column == nullptr) {
            Column *c = Column::makeColumn();
            c->addField(&(op->fields[pos]));
            op->write_set.insert(c);
        }
    }
}

void SchemaInference::visit(DAGReduceByKey *op) {
    DEBUG_PRINT("schema inference visiting reduce_by_key");

    // keep the key column
    DAGOperator *pred = dag()->predecessor(op);
    auto firstField = &(op->fields[0]);

    firstField->column = pred->fields[0].column;
    // add key to the read set
    op->read_set.insert(firstField->column);

    LLVMParser parser(op->llvm_ir);

    for (size_t i = 1; i < op->fields.size(); i++) {
        // TODO(sabir):
        // check which of the columns are read from the second input

        size_t arg_pos = i + op->fields.size() - 2;
        auto arg = dag()->predecessor(op)->fields[i];
        if (parser.is_argument_read(arg_pos)) {
            op->read_set.insert(arg.column);
        } else {
            // add to the dead set
            op->dead_set.insert(arg);
        }

        // generate a new column for every output
        Column *c = Column::makeColumn();
        c->addField(&(op->fields[i]));
        // add all cols to the write set
        op->write_set.insert(c);
    }
}

void SchemaInference::visit(DAGReduce *op) {
    LLVMParser parser(op->llvm_ir);

    for (size_t i = 0; i < op->fields.size(); i++) {
        // TODO(sabir):
        // check which of the columns are read from the second input

        size_t arg_pos = i + op->fields.size();
        auto arg = dag()->predecessor(op)->fields[i];
        if (parser.is_argument_read(arg_pos)) {
            op->read_set.insert(arg.column);
        } else {
            // add to the dead set
            op->dead_set.insert(arg);
        }

        // generate a new column for every output
        Column *c = Column::makeColumn();
        c->addField(&(op->fields[i]));
        // add all cols to the write set
        op->write_set.insert(c);
    }
}
