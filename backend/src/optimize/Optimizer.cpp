//
// Created by sabir on 29.08.17.
//

#include "Optimizer.h"

#include "SimplePredicateMoveAround.h"

#include "IR_analyzer/SchemaInference.h"

void Optimizer::run(DAG *dag) {
    // Schema inference
    SchemaInference si(dag);
    si.Run();

    // Move around filters
    SimplePredicateMoveAround opt(dag);
    opt.optimize();
}
