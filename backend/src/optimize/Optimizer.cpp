//
// Created by sabir on 29.08.17.
//

#include "Optimizer.h"

#include "assert_correct_open_next_close.hpp"
#include "attribute_id_tracking.hpp"
#include "canonicalize.hpp"
#include "create_pipelines.hpp"
#include "determine_sortedness.hpp"
#include "grouped_reduce_by_key.hpp"
#include "materialize_multiple_reads.hpp"
#include "simple_predicate_move_around.hpp"
#include "type_inference.hpp"

void Optimizer::run(DAG *dag) {
    // Verify tuple types
    optimize::TypeInference ti_check(true);
    optimize::TypeInference ti;
    ti_check.Run(dag);

    // Materialize results that are consumed multiple times
    optimize::MaterializeMultipleReads mmr;
    mmr.Run(dag);

    // Recompute output types
    ti.Run(dag);

    // Column tracking
    optimize::AttributeIdTracking ct;
    ct.Run(dag);
#ifndef NDEBUG
    ti_check.Run(dag);
#endif  // NDEBUG

    // Move around filters
    optimize::SimplePredicateMoveAround spma;
    spma.Run(dag);
#ifndef NDEBUG
    ti_check.Run(dag);
#endif  // NDEBUG

    // Determine sortedness
    optimize::DetermineSortedness sort;
    sort.Run(dag);
#ifndef NDEBUG
    ti_check.Run(dag);
#endif  // NDEBUG

    // Replace GroupByKey with grouped variant
    optimize::GroupedReduceByKey grbk;
    grbk.Run(dag);
#ifndef NDEBUG
    ti_check.Run(dag);
#endif  // NDEBUG

    // Split the plan into tree-shaped sub-plans
    optimize::CreatePipelines cp;
    cp.Run(dag);
    ti.Run(dag);
    sort.Run(dag);

#ifdef ASSERT_OPEN_NEXT_CLOSE
    optimize::AssertCorrectOpenNextClose acopn;
    acopn.Run(dag);
    ti.Run(dag);
#endif  // ASSERT_OPEN_NEXT_CLOSE

    // Make ID and order canonical
    optimize::Canonicalize canon;
    canon.Run(dag);
}
