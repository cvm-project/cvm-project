//
// Created by sabir on 29.08.17.
//

#include "Optimizer.h"

#include "assert_correct_open_next_close.hpp"
#include "attribute_id_tracking.hpp"
#include "determine_sortedness.hpp"
#include "grouped_reduce_by_key.hpp"
#include "materialize_multiple_reads.hpp"
#include "type_inference.hpp"

#include "SimplePredicateMoveAround.h"

void Optimizer::run(DAG *dag) {
    // Verify tuple types
    TypeInference ti_check(dag, true);
    TypeInference ti(dag);
    ti_check.optimize();

    // Materialize results that are consumed multiple times
    MaterializeMultipleReads mmr(dag);
    mmr.optimize();

    // Recompute output types
    ti.optimize();

    // Column tracking
    AttributeIdTracking ct(dag);
    ct.optimize();
#ifndef NDEBUG
    ti_check.optimize();
#endif  // NDEBUG

    // Move around filters
    SimplePredicateMoveAround spma(dag);
    spma.optimize();
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

#ifdef ASSERT_OPEN_NEXT_CLOSE
    AssertCorrectOpenNextClose acopn(dag);
    acopn.optimize();
    ti.optimize();
#endif  // ASSERT_OPEN_NEXT_CLOSE
}
