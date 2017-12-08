cmake_minimum_required(VERSION 3.8)
project(cpp)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/cmake/")

include_directories("${PROJECT_SOURCE_DIR}")

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

# Set up LLVMParser unit tests
add_executable(LLVMParser_tests
        tests/LLVMParser.cpp
        IR_analyzer/LLVMParser.cpp
    )
target_link_libraries(LLVMParser_tests PUBLIC
        ${LLVM_LIBS}
    )
add_test(LLVMParser LLVMParser_tests)
