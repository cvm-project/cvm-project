find_package(LLVM REQUIRED CONFIG)
project(testing)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES LLVMParser.cpp ../src/IR_analyzer/LLVMParser.cpp)



add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})


add_executable(${PROJECT_NAME} ${SOURCE_FILES})


llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(${PROJECT_NAME} PUBLIC ${llvm_libs} )