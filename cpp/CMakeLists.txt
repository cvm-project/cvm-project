cmake_minimum_required(VERSION 3.5)
project(cpp)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp src/code_gen/generate_code.cpp generate_dag_plan.cpp c_generate_dag_plan.c
        src/optimize/Optimizer.cpp src/optimize/SimplePredicateMoveAround.cpp
        src/dag/Column.cpp
        src/IR_analyzer/LLVMParser.cpp
        src/dag/DAGCreation.cpp
        src/dag/DAGOperator.cpp
        src/dag/DAGMap.cpp src/dag/DAGCollection.cpp src/dag/DAGRange.cpp src/dag/DAGFilter.cpp src/dag/DAGJoin.cpp
        src/dag/DAGReduce.cpp src/dag/DAGReduceByKey.cpp
        src/utils/utils.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

set(GRAPHVIZ_LIBS /usr/lib/libgvc.so /usr/lib/libcgraph.so /usr/lib/libcdt.so
        /opt/anaconda3/lib/graphviz/libgvplugin_core.so
        /opt/anaconda3/lib/graphviz/libgvplugin_dot_layout.so
        /opt/anaconda3/lib/graphviz/libgvplugin_neato_layout.so
        /opt/anaconda3/lib/graphviz/libgvplugin_pango.so
        /usr/lib/graphviz/libgvplugin_gd.so.6)

#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS_RELEASE}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(src/ ${LLVM_INCLUDE_DIRS})


add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_library(generate SHARED ${SOURCE_FILES} )
llvm_map_components_to_libnames(llvm_libs support core irreader)


# Link against LLVM libraries
#target_link_libraries(${PROJECT_NAME} PUBLIC ${llvm_libs} ${GRAPHVIZ_LIBS})
target_link_libraries(${PROJECT_NAME} PUBLIC  ${GRAPHVIZ_LIBS} ${llvm_libs} )
target_link_libraries(generate PUBLIC  ${GRAPHVIZ_LIBS} ${llvm_libs} )

