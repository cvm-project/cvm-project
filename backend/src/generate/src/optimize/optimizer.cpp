#include "optimize/optimizer.hpp"

#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "optimize/composite_transformation.hpp"
#include "optimize/dag_transformation.hpp"

namespace optimize {

Optimizer::Optimizer(std::string config) : config_(std::move(config)) {
    LoadDagTransformations();
}

void Optimizer::Run(DAG *const dag) {
    auto config = nlohmann::json::parse(config_).flatten();

    const bool verbose = config.value("/verbose", false);

    // Activate passes according to optimization level
    if (config.value("/optimization-level", 0) >= 1) {
        config.emplace("/optimizations/code_gen/active", true);
        config.emplace("/optimizations/canonicalize/active", true);
        config.emplace("/optimizations/create-pipelines/active", true);
        config.emplace("/optimizations/materialize-multiple-reads/active",
                       true);
        config.emplace("/optimizations/add-always-inline/active", true);
    }

    if (config.value("/optimization-level", 0) >= 2) {
        config.emplace("/optimizations/grouped-reduce-by-key/active", true);
        config.emplace("/optimizations/simple-predicate-move-around/active",
                       true);
    }

    // Assemble transformation pipeline
    std::vector<std::string> transformations;

    // Verify tuple types
    transformations.emplace_back("type_check");
#ifndef DEBUG
    transformations.emplace_back("verify");
#endif  // DEBUG

    // Materialize results that are consumed multiple times
    if (config.value("/optimizations/materialize-multiple-reads/active",
                     false)) {
        transformations.emplace_back("materialize_multiple_reads");
        transformations.emplace_back("type_inference");
#ifndef DEBUG
        transformations.emplace_back("verify");
#endif  // DEBUG
    }

    // Move around filters
    if (config.value("/optimizations/simple-predicate-move-around/active",
                     false)) {
        transformations.emplace_back("attribute_id_tracking");
#ifndef DEBUG
        transformations.emplace_back("type_check");
        transformations.emplace_back("verify");
#endif  // DEBUG

        transformations.emplace_back("simple_predicate_move_around");
#ifndef DEBUG
        transformations.emplace_back("type_check");
        transformations.emplace_back("verify");
#endif  // DEBUG
    }

    // Run target-specific optimization passes
    auto const target = config.value("/target", "singlecore");
    if (target == "omp") {
        transformations.emplace_back("parallelize");
        transformations.emplace_back("type_inference");
#ifndef DEBUG
        transformations.emplace_back("verify");
#endif  // DEBUG

        transformations.emplace_back("parallelize_omp");
        transformations.emplace_back("type_inference");
#ifndef DEBUG
        transformations.emplace_back("verify");
#endif  // DEBUG
    } else if (target != "singlecore") {
        throw std::invalid_argument("Unknown target: '" + target + "'");
    }

    // Replace GroupByKey with grouped variant
    if (config.value("/optimizations/grouped-reduce-by-key/active", false)) {
        transformations.emplace_back("attribute_id_tracking");
#ifndef DEBUG
        transformations.emplace_back("type_check");
        transformations.emplace_back("verify");
#endif  // DEBUG

        transformations.emplace_back("determine_sortedness");
#ifndef DEBUG
        transformations.emplace_back("type_check");
        transformations.emplace_back("verify");
#endif  // DEBUG

        transformations.emplace_back("grouped_reduce_by_key");
#ifndef DEBUG
        transformations.emplace_back("type_check");
        transformations.emplace_back("verify");
#endif  // DEBUG
    }

    // Add alwaysinline attribute to UDFs
    if (config.value("/optimizations/add-always-inline/active", false)) {
        transformations.emplace_back("add_always_inline");
    }

    if (config.value("/optimizations/create-pipelines/active", false)) {
        // Split the plan into tree-shaped sub-plans
        transformations.emplace_back("create_pipelines");
        transformations.emplace_back("type_inference");
        transformations.emplace_back("determine_sortedness");
#ifndef DEBUG
        transformations.emplace_back("verify");
#endif  // DEBUG
    }

    // Assert that operators implement Open-Next-Close interface correctly
    if (config.value("/optimizations/assert-correct-open-next-close/active",
                     false)) {
        transformations.emplace_back("assert_correct_open_next_close");
        transformations.emplace_back("type_inference");
#ifndef DEBUG
        transformations.emplace_back("verify");
#endif  // DEBUG
    }

    // Make ID and order canonical
    if (config.value("/optimizations/canonicalize/active", false)) {
        transformations.emplace_back("canonicalize");
#ifndef DEBUG
        transformations.emplace_back("verify");
#endif  // DEBUG
    }

    // Just-in-Time compilation
    if (config.value("/optimizations/code_gen/active", false)) {
        transformations.emplace_back("code_gen");
        transformations.emplace_back("type_inference");
        transformations.emplace_back("canonicalize");
#ifndef DEBUG
        transformations.emplace_back("verify");
#endif  // DEBUG
    }

    if (verbose) {
        printf("Running the following transformations:\n");
        for (auto const &t : transformations) {
            printf("  %s\n", t.c_str());
        }
    }

    // Run pipeline
    CompositeTransformation pipeline(transformations);
    pipeline.Run(dag, config.unflatten()
                              .value("optimizations", nlohmann::json::object())
                              .dump());
}

}  // namespace optimize
