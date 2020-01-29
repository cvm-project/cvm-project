add_library(generate SHARED
        src/code_gen/code_gen.cpp
        src/code_gen/code_gen_visitor.cpp
        src/code_gen/context.cpp
        src/generate/generate_executable.cpp
        src/llvm_helpers/function.cpp
        src/optimize/add_always_inline.cpp
        src/optimize/code_gen.cpp
        src/optimize/assert_correct_open_next_close.cpp
        src/optimize/attribute_id_tracking.cpp
        src/optimize/canonicalize.cpp
        src/optimize/composite_transformation.cpp
        src/optimize/create_pipelines.cpp
        src/optimize/dag_transformation.cpp
        src/optimize/determine_sortedness.cpp
        src/optimize/grouped_reduce_by_key.cpp
        src/optimize/materialize_multiple_reads.cpp
        src/optimize/optimizer.cpp
        src/optimize/parallelize.cpp
        src/optimize/parallelize_omp.cpp
        src/optimize/type_inference.cpp
        src/optimize/grouped_reduce_by_key.cpp
        src/optimize/simple_predicate_move_around.cpp
        src/optimize/utils.cpp
        src/optimize/verify.cpp
    )

target_include_directories(generate
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_link_libraries(generate
    PUBLIC
        LLVMCore
        LLVMIRReader
        LLVMSupport
        dag
    PRIVATE
        Boost::boost
        Boost::filesystem
        Boost::system
        nlohmann_json::nlohmann_json
        polymorphic_value::polymorphic_value
        runtime
        utils
    )

add_executable(llvm_helpers_tests
        tests/llvm_helpers_function.cpp
    )

target_link_libraries(llvm_helpers_tests
    PRIVATE
        generate
        Catch2::Catch2
    )

add_test(NAME llvm_helpers
        COMMAND llvm_helpers_tests
    )
