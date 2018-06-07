//
// Created by sabir on 29.08.17.
//

#include "Optimizer.h"

#include "column_tracking.hpp"
#include "determine_sortedness.hpp"
#include "grouped_reduce_by_key.hpp"

#include "SimplePredicateMoveAround.h"

#include "IR_analyzer/SchemaInference.h"

void Optimizer::run(DAG *dag) {
    // Schema inference
    SchemaInference si(dag);
    si.Run();

    // Column tracking
    ColumnTracking ct(dag);
    ct.optimize();

    // Move around filters
    SimplePredicateMoveAround opt(dag);
    opt.optimize();

    // Determine sortedness
    DetermineSortedness sort(dag);
    sort.optimize();

    // Replace GroupByKey with grouped variant
    GroupedReduceByKey grbk(dag);
    grbk.optimize();
}
