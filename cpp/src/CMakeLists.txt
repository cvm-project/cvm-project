cmake_minimum_required(VERSION 3.8)
project(cpp)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/cmake/")

include_directories("${PROJECT_SOURCE_DIR}")

include (ReleaseAsDefault)

# Setup clang-format
find_package (ClangFormat 4.0)
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

# Set up library
set(SOURCE_FILES
        generate_dag_plan.cpp
        code_gen/generate_code.cpp
        dag/Column.cpp
        dag/DAGCartesian.cpp
        dag/DAGCollection.cpp
        dag/DAGCreation.cpp
        dag/DAGFilter.cpp
        dag/DAGJoin.cpp
        dag/DAGMap.cpp
        dag/DAGOperator.cpp
        dag/DAGRange.cpp
        dag/DAGReduce.cpp
        dag/DAGReduceByKey.cpp
        IR_analyzer/LLVMParser.cpp
        optimize/Optimizer.cpp
        optimize/SimplePredicateMoveAround.cpp
        utils/c_type_to_llvm.cpp
        utils/printDAG.cpp
        utils/utils.cpp
    )
add_library(generate SHARED ${SOURCE_FILES} )
target_link_libraries(generate PUBLIC
        ${GRAPHVIZ_GVC_LIBRARY}
        ${LLVM_LIBS}
    )

# Set up main executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC
        ${GRAPHVIZ_GVC_LIBRARY}
        ${LLVM_LIBS}
        generate
    )

# Set up benchmark utils
add_executable(benchmark benchmarks/main.cpp)
add_test(benchmark benchmark)

add_executable(max_memory benchmarks/max_memory.c)

# Set up LLVMParser unit tests
add_executable(LLVMParser_tests
        tests/LLVMParser.cpp
        IR_analyzer/LLVMParser.cpp
        utils/c_type_to_llvm.cpp
    )
target_link_libraries(LLVMParser_tests PUBLIC
        ${LLVM_LIBS}
    )
add_test(LLVMParser LLVMParser_tests)

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
