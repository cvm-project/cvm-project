//
// Created by sabir on 29.08.17.
//

#include "Optimizer.h"

#include "attribute_id_tracking.hpp"
#include "determine_sortedness.hpp"
#include "grouped_reduce_by_key.hpp"
#include "type_inference.hpp"

#include "SimplePredicateMoveAround.h"

void Optimizer::run(DAG *dag) {
    // Type inference
    TypeInference ti(dag);
    ti.optimize();

    // Column tracking
    AttributeIdTracking ct(dag);
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
