//
// Created by sabir on 19.07.17.
//

#include "generate_code.h"

void generate_code(DAG *dag) {
    CodeGenVisitor visitor;
    visitor.start_visit(dag);
}