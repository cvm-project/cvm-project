//
// Created by sabir on 29.08.17.
//

#include "optimizer.hpp"

#include <string>
#include <vector>

#include <json.hpp>

#include "composite_transformation.hpp"
#include "dag_transformation.hpp"

namespace optimize {

// cppcheck-suppress passedByValue
Optimizer::Optimizer(std::string config) : config_(std::move(config)) {
    LoadDagTransformations();
}
void Optimizer::Run(DAG *const dag) {
    auto config = nlohmann::json::parse(config_).at("optimizer").flatten();

    if (config.value("/optimization-level", 0) <= 0) {
        return;
    }

    std::vector<std::string> transformations;

    // Verify tuple types
    transformations.emplace_back("type_check");

    // Materialize results that are consumed multiple times
    transformations.emplace_back("materialize_multiple_reads");
    transformations.emplace_back("type_inference");

    // Column tracking
    transformations.emplace_back("attribute_id_tracking");
#ifndef NDEBUG
    transformations.emplace_back("type_check");
#endif  // NDEBUG

    // Move around filters
    transformations.emplace_back("simple_predicate_move_around");
#ifndef NDEBUG
    transformations.emplace_back("type_check");
#endif  // NDEBUG

    // Determine sortedness
    transformations.emplace_back("determine_sortedness");
#ifndef NDEBUG
    transformations.emplace_back("type_check");
#endif  // NDEBUG

    // Replace GroupByKey with grouped variant
    transformations.emplace_back("grouped_reduce_by_key");
#ifndef NDEBUG
    transformations.emplace_back("type_check");
#endif  // NDEBUG

    // Split the plan into tree-shaped sub-plans
    transformations.emplace_back("create_pipelines");
    transformations.emplace_back("type_inference");
    transformations.emplace_back("determine_sortedness");

#ifdef ASSERT_OPEN_NEXT_CLOSE
    transformations.emplace_back("assert_correct_open_next_close");
    transformations.emplace_back("type_inference");
#endif  // ASSERT_OPEN_NEXT_CLOSE

    // Make ID and order canonical
    transformations.emplace_back("canonicalize");

    CompositeTransformation pipeline(transformations);
    pipeline.Run(dag);
}

}  // namespace optimize
