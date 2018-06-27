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
    // Verify tuple types
    TypeInference ti_check(dag, true);
    ti_check.optimize();

    // Column tracking
    AttributeIdTracking ct(dag);
    ct.optimize();
#ifndef NDEBUG
    ti_check.optimize();
#endif  // NDEBUG

    // Move around filters
    SimplePredicateMoveAround opt(dag);
    opt.optimize();
#ifndef NDEBUG
    ti_check.optimize();
#endif  // NDEBUG

    // Determine sortedness
    DetermineSortedness sort(dag);
    sort.optimize();
#ifndef NDEBUG
    ti_check.optimize();
#endif  // NDEBUG

    // Replace GroupByKey with grouped variant
    GroupedReduceByKey grbk(dag);
    grbk.optimize();
#ifndef NDEBUG
    ti_check.optimize();
#endif  // NDEBUG
}
