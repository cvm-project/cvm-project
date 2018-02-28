//
// Created by sabir on 29.08.17.
//

#include "Optimizer.h"
#include "SimplePredicateMoveAround.h"

void Optimizer::run(DAG *dag) {
    SimplePredicateMoveAround opt;
    opt.optimize(dag);
}