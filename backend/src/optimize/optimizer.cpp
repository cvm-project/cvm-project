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

    // Activate passes according to optimization level
    if (config.value("/optimization-level", 0) <= 0) {
        return;
    }

    if (config.value("/optimization-level", 0) >= 1) {
        config.emplace("/optimizations/canonicalize", true);
        config.emplace("/optimizations/create-pipelines", true);
        config.emplace("/optimizations/materialize-multiple-reads", true);
    }

    if (config.value("/optimization-level", 0) >= 2) {
        config.emplace("/optimizations/grouped-reduce-by-key", true);
        config.emplace("/optimizations/simple-predicate-move-around", true);
    }

    // Assemble transformation pipeline
    std::vector<std::string> transformations;

    // Verify tuple types
    transformations.emplace_back("type_check");

    // Materialize results that are consumed multiple times
    if (config.value("/optimizations/materialize-multiple-reads", false)) {
        transformations.emplace_back("materialize_multiple_reads");
        transformations.emplace_back("type_inference");
    }

    // Move around filters
    if (config.value("/optimizations/simple-predicate-move-around", false)) {
        transformations.emplace_back("attribute_id_tracking");
#ifndef NDEBUG
        transformations.emplace_back("type_check");
#endif  // NDEBUG

        transformations.emplace_back("simple_predicate_move_around");
#ifndef NDEBUG
        transformations.emplace_back("type_check");
#endif  // NDEBUG
    }

    // Replace GroupByKey with grouped variant
    if (config.value("/optimizations/grouped-reduce-by-key", false)) {
        transformations.emplace_back("attribute_id_tracking");
#ifndef NDEBUG
        transformations.emplace_back("type_check");
#endif  // NDEBUG

        transformations.emplace_back("determine_sortedness");
#ifndef NDEBUG
        transformations.emplace_back("type_check");
#endif  // NDEBUG

        transformations.emplace_back("grouped_reduce_by_key");
#ifndef NDEBUG
        transformations.emplace_back("type_check");
#endif  // NDEBUG
    }

    if (config.value("/optimizations/create-pipelines", false)) {
        // Split the plan into tree-shaped sub-plans
        transformations.emplace_back("create_pipelines");
        transformations.emplace_back("type_inference");
        transformations.emplace_back("determine_sortedness");
    }

    // Assert that operators implement Open-Next-Close interface correctly
    if (config.value("/optimizations/assert-correct-open-next-close", false)) {
        transformations.emplace_back("assert_correct_open_next_close");
        transformations.emplace_back("type_inference");
    }

    // Make ID and order canonical
    if (config.value("/optimizations/canonicalize", false)) {
        transformations.emplace_back("canonicalize");
    }

    // Run pipeline
    CompositeTransformation pipeline(transformations);
    pipeline.Run(dag);
}

}  // namespace optimize
