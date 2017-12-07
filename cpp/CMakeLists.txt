cmake_minimum_required(VERSION 3.8)
project(cpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../3rdparty/cmake/")

include (ReleaseAsDefault)

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

# Set up compilation options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -ffast-math")

# Set up library
include_directories(src/)
set(SOURCE_FILES
        c_generate_dag_plan.c
        generate_dag_plan.cpp
        src/code_gen/generate_code.cpp
        src/dag/Column.cpp
        src/dag/DAGCartesian.cpp
        src/dag/DAGCollection.cpp
        src/dag/DAGCreation.cpp
        src/dag/DAGFilter.cpp
        src/dag/DAGJoin.cpp
        src/dag/DAGMap.cpp
        src/dag/DAGOperator.cpp
        src/dag/DAGRange.cpp
        src/dag/DAGReduce.cpp
        src/dag/DAGReduceByKey.cpp
        src/IR_analyzer/LLVMParser.cpp
        src/optimize/Optimizer.cpp
        src/optimize/SimplePredicateMoveAround.cpp
        src/utils/utils.cpp
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
