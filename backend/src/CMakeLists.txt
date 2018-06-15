cmake_minimum_required(VERSION 3.8)
project(cpp)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/cmake/")

include_directories("${PROJECT_SOURCE_DIR}")

include (ReleaseAsDefault)

# Setup clang-format
find_package (ClangFormat 5.0 EXACT)
if(ClangFormat_FOUND)
    enable_clang_format_targets()
endif()

# Set up LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(LLVM_LIBS support core irreader)

# Set up GraphViz
find_package(GraphViz REQUIRED)
include_directories(${GRAPHVIZ_INCLUDE_DIRS})

# Set up Catch (for unit tests)
set(CATCH_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../3rdparty/catch/single_include")
get_filename_component(CATCH_INCLUDE_DIRS "${CATCH_INCLUDE_DIRS}" ABSOLUTE)
include_directories(${CATCH_INCLUDE_DIRS})

# Set up JSON library
set(JSONLIB_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../3rdparty/json/src")
get_filename_component(JSONLIB_INCLUDE_DIRS "${JSONLIB_INCLUDE_DIRS}" ABSOLUTE)
include_directories(${JSONLIB_INCLUDE_DIRS})

# Set up compilation options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -ffast-math")

# Set up Boost
find_package(Boost 1.58 REQUIRED COMPONENTS
        program_options
    )
include_directories(${Boost_INCLUDE_DIRS})

# Set up library
set(SOURCE_FILES
        generate_dag_plan.cpp
        code_gen/cpp/CodeGenVisitor.cpp
        code_gen/cpp/BackEnd.cpp
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
        IR_analyzer/LLVMParser.cpp
        IR_analyzer/llvm_field_type_visitor.cpp
        optimize/attribute_id_tracking.cpp
        optimize/determine_sortedness.cpp
        optimize/Optimizer.cpp
        optimize/grouped_reduce_by_key.cpp
        optimize/SimplePredicateMoveAround.cpp
        utils/c_type_to_llvm.cpp
        utils/DAGVisitor.cpp
        utils/utils.cpp
         )
add_library(generate SHARED ${SOURCE_FILES} )
target_link_libraries(generate PUBLIC
        ${LLVM_LIBS}
    )

# Set up benchmark utils
add_executable(benchmark benchmarks/main.cpp)

add_executable(max_memory benchmarks/max_memory.c)

# Set up LLVMParser unit tests
add_executable(LLVMParser_tests
        tests/LLVMParser.cpp
    )
target_link_libraries(LLVMParser_tests PUBLIC
        generate
    )
add_test(LLVMParser LLVMParser_tests)

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
            "--suppress='*:${CATCH_INCLUDE_DIRS}/*'"
            "--suppress='*:${JSONLIB_INCLUDE_DIRS}/*'"
        )
endif()
