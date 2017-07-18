cmake_minimum_required(VERSION 3.5)
project(cpp)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp main.cpp src/operators/Operator.h src/dag/DAGOperator.h src/dag/DAGFilter.cpp src/dag/DAGFilter.h
        src/dag/DAGRange.cpp src/dag/DAGRange.h src/dag/DAGMap.cpp src/dag/DAGMap.h src/dag/DAGCreation.cpp
        src/dag/DAGCollection.cpp
        src/operators/FilterOperator.cpp src/operators/FilterOperator.h src/operators/MapOperator.cpp
        src/operators/MapOperator.h src/operators/RangeSourceOperator.cpp src/operators/RangeSourceOperator.h
        src/dag/DAG.h src/operators/JoinOperator.cpp src/operators/JoinOperator.h
        src/IR_analyzer/Parser.cpp src/IR_analyzer/Parser.h src/gen/BuildDag.cpp src/gen/BuildDag.h src/libs/optional.h src/libs/optional.h src/function_template.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(src/ ${LLVM_INCLUDE_DIRS})
#include_directories(src/)
add_executable(cpp ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs support core irreader)
# Link against LLVM libraries
target_link_libraries(cpp ${llvm_libs})
