cmake_minimum_required(VERSION 3.8)
project(cpp)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/cmake/")

include_directories("${PROJECT_SOURCE_DIR}")

include (ReleaseAsDefault)
include (ExternalProject)

# Setup clang-format
find_package (ClangFormat 5.0 EXACT)
if(ClangFormat_FOUND)
    enable_clang_format_targets()
endif()

# Setup clang-tidy
find_package (ClangTidy 5.0 EXACT)
if(ClangTidy_FOUND)
    enable_clang_tidy_targets()
endif()

# Set up LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(LLVM_LIBS support core irreader)

# Set up GraphViz
find_package(GraphViz REQUIRED)
include_directories(SYSTEM ${GRAPHVIZ_INCLUDE_DIRS})

# Set up Catch (for unit tests)
set(CATCH_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../3rdparty/catch/single_include")
get_filename_component(CATCH_INCLUDE_DIRS "${CATCH_INCLUDE_DIRS}" ABSOLUTE)
include_directories(SYSTEM ${CATCH_INCLUDE_DIRS})

# Set up JSON library
set(JSONLIB_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../3rdparty/json/src")
get_filename_component(JSONLIB_INCLUDE_DIRS "${JSONLIB_INCLUDE_DIRS}" ABSOLUTE)
include_directories(SYSTEM ${JSONLIB_INCLUDE_DIRS})

# Set up compilation options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -ffast-math")

# Set up Boost
find_package(Boost 1.58 REQUIRED COMPONENTS
        program_options
    )
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Set up google unit test framework
ExternalProject_Add(googletest
        SOURCE_DIR      "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/googletest"
        PREFIX          "${CMAKE_BINARY_DIR}/googletest"
        TEST_COMMAND    ""
        CMAKE_ARGS      "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER};-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER};-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/googletest"
    )

set(GTEST_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/googletest/include")
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

ExternalProject_Get_Property(googletest INSTALL_DIR)
set(GTEST_INSTALL_DIR ${INSTALL_DIR})

set(GTEST_LIBRARY_GTEST gtest)
set(GTEST_LIBRARY_GTESTMAIN gtest_main)
set(GTEST_LIBRARY_GTEST_PATH "${GTEST_INSTALL_DIR}/lib/libgtest.a")
set(GTEST_LIBRARY_GTESTMAIN_PATH "${GTEST_INSTALL_DIR}/lib/libgtest_main.a")
add_library(${GTEST_LIBRARY_GTEST} UNKNOWN IMPORTED)
add_library(${GTEST_LIBRARY_GTESTMAIN} UNKNOWN IMPORTED)
set_property(TARGET ${GTEST_LIBRARY_GTEST} PROPERTY IMPORTED_LOCATION
    ${GTEST_LIBRARY_GTEST_PATH} )
set_property(TARGET ${GTEST_LIBRARY_GTESTMAIN} PROPERTY IMPORTED_LOCATION
    ${GTEST_LIBRARY_GTESTMAIN_PATH} )
add_dependencies(${GTEST_LIBRARY_GTEST} googletest)
add_dependencies(${GTEST_LIBRARY_GTESTMAIN} googletest)
set(GTEST_LIBRARIES
        ${GTEST_LIBRARY_GTEST}
        ${GTEST_LIBRARY_GTESTMAIN}
        pthread
)

include(GoogleTest)

# Set up generate library
set(GENERATE_SOURCE_FILES
        code_gen/cpp/BackEnd.cpp
        code_gen/cpp/code_gen.cpp
        code_gen/cpp/CodeGenVisitor.cpp
        code_gen/cpp/context.cpp
        compiler/compiler.cpp
        compiler/generate_executable.cpp
        dag/attribute_id.cpp
        dag/DAGConstantTuple.cpp
        dag/DAG.cpp
        dag/DAGCollection.cpp
        dag/dag_factory.cpp
        dag/DAGFilter.cpp
        dag/DAGOperator.cpp
        dag/collection/field.cpp
        dag/collection/array.cpp
        dag/collection/tuple.cpp
        dag/type/array.cpp
        dag/type/atomic.cpp
        dag/type/field_type.cpp
        dag/type/type.cpp
        dag/type/tuple.cpp
        dag/utils/apply_visitor.cpp
        llvm_helpers/function.cpp
        optimize/assert_correct_open_next_close.cpp
        optimize/attribute_id_tracking.cpp
        optimize/determine_sortedness.cpp
        optimize/materialize_multiple_reads.cpp
        optimize/Optimizer.cpp
        optimize/type_inference.cpp
        optimize/grouped_reduce_by_key.cpp
        optimize/SimplePredicateMoveAround.cpp
        utils/c_type_to_llvm.cpp
        utils/utils.cpp
    )
add_library(generate SHARED ${GENERATE_SOURCE_FILES} )
target_link_libraries(generate PUBLIC
        ${LLVM_LIBS}
        runtime
    )

# Set up runtime library
set(RUNTIME_SOURCE_FILES
        runtime/free.cpp
        runtime/values/json_parsing.cpp
        runtime/values/array_json.cpp
        runtime/values/tuple_json.cpp
        runtime/values/value_json.cpp
        runtime/values/value_parser.cpp
    )
add_library(runtime SHARED ${RUNTIME_SOURCE_FILES} )

# Set up benchmark utils
add_executable(benchmark benchmarks/main.cpp)

add_executable(max_memory benchmarks/max_memory.c)

# Set up llvm_function unit tests
add_executable(llvm_helpers_tests
        tests/llvm_helpers_function.cpp
    )
target_link_libraries(llvm_helpers_tests PUBLIC
        generate
    )
add_test(llvm_helpers llvm_helpers_tests)

# Set up optimizer command line tool
add_executable(opt-cli
        tools/opt.cpp
        utils/printDAG.cpp
    )
set_target_properties(opt-cli PROPERTIES
        OUTPUT_NAME "opt"
    )
target_link_libraries(opt-cli PUBLIC
        ${GRAPHVIZ_CGRAPH_LIBRARY}
        ${GRAPHVIZ_GVC_LIBRARY}
        ${Boost_LIBRARIES}
        generate
    )

# Setup cppcheck
find_package(cppcheck 1.81)
if(cppcheck_FOUND)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(
        cppcheck
        COMMAND ${cppcheck_EXECUTABLE}
            "--project=compile_commands.json"
            "--enable=warning,performance,style"
            "--error-exitcode=1"
            "--inline-suppr"
            "--suppress='*:${LLVM_INCLUDE_DIRS}/*'"
            "--suppress='*:${GRAPHVIZ_INCLUDE_DIRS}/*'"
            "--suppress='*:${GTEST_INCLUDE_DIRS}/*'"
            "--suppress='*:${CATCH_INCLUDE_DIRS}/*'"
            "--suppress='*:${JSONLIB_INCLUDE_DIRS}/*'"
        )
endif()

# Set up google unit test target
add_executable(gtest_tests
        tests/visitor_test.cpp
    )
target_link_libraries(gtest_tests "${GTEST_LIBRARIES}")
gtest_discover_tests(gtest_tests)
