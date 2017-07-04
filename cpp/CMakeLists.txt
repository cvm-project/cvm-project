cmake_minimum_required(VERSION 3.6)
project(cpp)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp main.cpp src/operators/Operator.h src/dag/DAGOperator.h src/dag/DAGFilter.cpp src/dag/DAGFilter.h src/dag/DAGRange.cpp src/dag/DAGRange.h src/dag/DAGMap.cpp src/dag/DAGMap.h src/dag/DAGCreation.cpp src/operators/FilterOperator.cpp src/operators/FilterOperator.h src/operators/MapOperator.cpp src/operators/MapOperator.h src/operators/RangeSourceOperator.cpp src/operators/RangeSourceOperator.h src/dag/DAG.cpp src/dag/DAG.h src/operators/JoinOperator.cpp src/operators/JoinOperator.h)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
include_directories(src/)
add_executable(cpp ${SOURCE_FILES})