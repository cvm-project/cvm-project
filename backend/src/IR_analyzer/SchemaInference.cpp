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

void SchemaInference::visit(DAGCollection * /*op*/) {
    DEBUG_PRINT("schema inference visiting collection");
}

void SchemaInference::visit(DAGRange * /*op*/) {
    DEBUG_PRINT("schema inference visiting range");
}

void SchemaInference::visit(DAGFilter *op) {
    DEBUG_PRINT("schema inference visiting filter");

    // copy output columns and fields from the predecessor
    for (size_t i = 0; i < op->fields.size(); i++) {
        Column *col = dag()->predecessor(op)->fields[i].column;
        op->fields[i].column = col;
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
        for (auto pos : parser.get_output_positions(c)) {
            // every output in the list now has the same column type as this
            // input
            op->fields[pos].column = arg.column;
            arg.column->addField(&(op->fields[pos]));
        }
        c++;
    }
}

void SchemaInference::visit(DAGReduceByKey *op) {
    DEBUG_PRINT("schema inference visiting reduce_by_key");

    // keep the key column
    DAGOperator *pred = dag()->predecessor(op);
    auto firstField = &(op->fields[0]);

    firstField->column = pred->fields[0].column;
}

void SchemaInference::visit(DAGReduce * /*op*/) {}
