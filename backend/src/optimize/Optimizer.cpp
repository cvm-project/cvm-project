//
// Created by sabir on 29.08.17.
//

#include "Optimizer.h"

#include "assert_correct_open_next_close.hpp"
#include "attribute_id_tracking.hpp"
#include "canonicalize.hpp"
#include "create_pipelines.hpp"
#include "dag_transformation.hpp"
#include "determine_sortedness.hpp"
#include "grouped_reduce_by_key.hpp"
#include "materialize_multiple_reads.hpp"
#include "simple_predicate_move_around.hpp"
#include "type_inference.hpp"

void Optimizer::run(DAG *dag) {
    std::vector<std::unique_ptr<const optimize::DagTransformation>>
            transformations;

    // Verify tuple types
    transformations.emplace_back(new optimize::TypeInference(true));

    // Materialize results that are consumed multiple times
    transformations.emplace_back(new optimize::MaterializeMultipleReads);
    transformations.emplace_back(new optimize::TypeInference);

    // Column tracking
    transformations.emplace_back(new optimize::AttributeIdTracking);
#ifndef NDEBUG
    transformations.emplace_back(new optimize::TypeInference(true));
#endif  // NDEBUG

    // Move around filters
    transformations.emplace_back(new optimize::SimplePredicateMoveAround);
#ifndef NDEBUG
    transformations.emplace_back(new optimize::TypeInference(true));
#endif  // NDEBUG

    // Determine sortedness
    transformations.emplace_back(new optimize::DetermineSortedness);
#ifndef NDEBUG
    transformations.emplace_back(new optimize::TypeInference(true));
#endif  // NDEBUG

    // Replace GroupByKey with grouped variant
    transformations.emplace_back(new optimize::GroupedReduceByKey);
#ifndef NDEBUG
    transformations.emplace_back(new optimize::TypeInference(true));
#endif  // NDEBUG

    // Split the plan into tree-shaped sub-plans
    transformations.emplace_back(new optimize::CreatePipelines);
    transformations.emplace_back(new optimize::TypeInference);
    transformations.emplace_back(new optimize::DetermineSortedness);

#ifdef ASSERT_OPEN_NEXT_CLOSE
    transformations.emplace_back(new optimize::AssertCorrectOpenNextClose);
    transformations.emplace_back(new optimize::TypeInference);
#endif  // ASSERT_OPEN_NEXT_CLOSE

    // Make ID and order canonical
    transformations.emplace_back(new optimize::Canonicalize);

    for (auto const &transformation : transformations) {
        transformation->Run(dag);
    }
}
