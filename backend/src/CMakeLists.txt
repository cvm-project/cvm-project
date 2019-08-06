cmake_minimum_required(VERSION 3.8)
project(cpp)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/cmake/")

set(LLVM_ASAN OFF CACHE BOOL "Enable compilation with LLVM address, undefined, and leak sanitizer.")

include_directories("${PROJECT_SOURCE_DIR}")

include (ReleaseAsDefault)
include (ExternalProject)

# Setup clang-format
find_package (ClangFormat 7.0 EXACT)
if(ClangFormat_FOUND)
    enable_clang_format_targets()
endif()

# Setup clang-tidy
find_package (ClangTidy 7.0 EXACT)
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

# Set up Catch (for unit tests)
set(CATCH_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../3rdparty/catch/single_include")
get_filename_component(CATCH_INCLUDE_DIRS "${CATCH_INCLUDE_DIRS}" ABSOLUTE)
include_directories(SYSTEM ${CATCH_INCLUDE_DIRS})

# Set up JSON library
set(JSON_ROOT_DIRECTORY "${PROJECT_SOURCE_DIR}/../../3rdparty/json/")
get_filename_component(JSON_ROOT_DIRECTORY "${JSON_ROOT_DIRECTORY}" ABSOLUTE)
set(JSON_BuildTests OFF CACHE BOOL
    "Build the unit tests when BUILD_TESTING is enabled.")
add_subdirectory("${JSON_ROOT_DIRECTORY}" nlohmann_json)

# Set up pybind11
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/pybind11" pybind11)
set(PYBIND11_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/pybind11/include")
get_filename_component(PYBIND11_INCLUDE_DIRS "${PYBIND11_INCLUDE_DIRS}" ABSOLUTE)
include_directories(SYSTEM "{$PYBIND11_INCLUDE_DIRS}")

# Set up polymorphic_value
set(POLYMORPHIC_ROOT_DIR "${PROJECT_SOURCE_DIR}/../../3rdparty/polymorphic_value")
get_filename_component(POLYMORPHIC_ROOT_DIR "${POLYMORPHIC_ROOT_DIR}" ABSOLUTE)
add_subdirectory("${POLYMORPHIC_ROOT_DIR}" polymorphic_value)

# Set up compilation options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -ffast-math")

# Set up sanitizers
if(${LLVM_ASAN} AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "Enabling LLVM's sanitizers.")
    add_compile_definitions("LLVM_ASAN")
    set(LLVM_LSAN_FLAGS "-fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize=vptr")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLVM_LSAN_FLAGS}")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} ${LLVM_LSAN_FLAGS}")
endif()

# Set up Boost
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.64 REQUIRED COMPONENTS
        filesystem
        program_options
        system
    )
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
add_compile_definitions("BOOST_MPL_LIMIT_LIST_SIZE=30")
add_compile_definitions("BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS")

# Set up skyr URL library
ExternalProject_Add(Skyr
        URL             "https://github.com/cpp-netlib/url/archive/v1.1.tar.gz"
        PREFIX          "${CMAKE_BINARY_DIR}/skyr-url"
        TEST_COMMAND    ""
        CMAKE_ARGS      "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER};"
                        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER};"
                        "-DCMAKE_CXX_FLAGS=-fPIC;"
                        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/skyr-url;"
                        "-DSkyr_BUILD_TESTS=OFF;"
                        "-DSkyr_DISABLE_LIBCXX=ON;"
                        "-DSkyr_BUILD_FILESYSTEM_PATH_FUNCTIONS=OFF"
    )
ExternalProject_Get_Property(Skyr INSTALL_DIR)
set(Skyr_INSTALL_DIR ${INSTALL_DIR})

set(Skyr_INCLUDE_DIR "${Skyr_INSTALL_DIR}/include")
include_directories(SYSTEM ${Skyr_INCLUDE_DIR})

set(Skyr_LIBRARY skyr-url)
set(Skyr_LIBRARY_PATH "${Skyr_INSTALL_DIR}/lib/libskyr-url.a")
add_library(${Skyr_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${Skyr_LIBRARY} PROPERTY IMPORTED_LOCATION
    ${Skyr_LIBRARY_PATH} )
add_dependencies(${Skyr_LIBRARY} Skyr)

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

# Set up arrow
find_package(arrow REQUIRED CONFIG)
message(STATUS "Found arrow ${arrow_VERSION}")
set(arrow_LIBRARY arrow_shared)
set(parquet_LIBRARY parquet_shared)

# Set up AWS SDK
find_package(AWSSDK CONFIG COMPONENTS
        s3
    )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(dag)
add_subdirectory(tools)

# Set up generate library
set(GENERATE_SOURCE_FILES
        code_gen/cpp/code_gen.cpp
        code_gen/cpp/CodeGenVisitor.cpp
        code_gen/cpp/context.cpp
        generate_executable.cpp
        llvm_helpers/function.cpp
        optimize/add_always_inline.cpp
        optimize/code_gen.cpp
        optimize/assert_correct_open_next_close.cpp
        optimize/attribute_id_tracking.cpp
        optimize/canonicalize.cpp
        optimize/composite_transformation.cpp
        optimize/create_pipelines.cpp
        optimize/dag_transformation.cpp
        optimize/determine_sortedness.cpp
        optimize/grouped_reduce_by_key.cpp
        optimize/materialize_multiple_reads.cpp
        optimize/optimizer.cpp
        optimize/parallelize.cpp
        optimize/type_inference.cpp
        optimize/grouped_reduce_by_key.cpp
        optimize/simple_predicate_move_around.cpp
        utils/c_type_to_llvm.cpp
        utils/utils.cpp
    )
add_library(generate SHARED ${GENERATE_SOURCE_FILES} )
target_link_libraries(generate PUBLIC
        nlohmann_json::nlohmann_json
        polymorphic_value::polymorphic_value
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${LLVM_LIBS}
        dag
        runtime
    )

# Set up Python module
pybind11_add_module(jitq_backend
        python_module.cpp
    )
target_link_libraries(jitq_backend PUBLIC
        generate
        runtime
    )

# Set up runtime library
set(RUNTIME_SOURCE_FILES
        runtime/execute_plan.cpp
        runtime/filesystem/file.cpp
        runtime/filesystem/filesystem.cpp
        runtime/memory/shared_pointer.cpp
        runtime/memory/values.cpp
        runtime/operators/arrow_helpers.cpp
        runtime/operators/expand_pattern.cpp
        runtime/operators/materialize_parquet_impl.cpp
        runtime/operators/parquet_scan_impl.cpp
        runtime/operators/value_to_record_batch.cpp
        runtime/values/json_parsing.cpp
        runtime/values/array_json.cpp
        runtime/values/tuple_json.cpp
        runtime/values/value_json.cpp
        runtime/values/value_parser.cpp
    )
if(AWSSDK_FOUND)
    set(RUNTIME_SOURCE_FILES ${RUNTIME_SOURCE_FILES}
            runtime/aws/aws.cpp
            runtime/aws/s3.cpp
            runtime/filesystem/s3.cpp
        )
endif()
add_library(runtime SHARED ${RUNTIME_SOURCE_FILES} )
target_link_libraries(runtime PUBLIC
        nlohmann_json::nlohmann_json
        ${Skyr_LIBRARY}
        ${AWSSDK_LINK_LIBRARIES}
        ${arrow_LIBRARY}
        ${parquet_LIBRARY}
    )

# Set up dummy dlclose library
add_library(dlclose_noop SHARED tools/dlclose_noop.c)

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

# Setup cppcheck
find_package(cppcheck 1.81 EXACT)
if(cppcheck_FOUND)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(
        cppcheck
        COMMAND ${cppcheck_EXECUTABLE}
            "--project=compile_commands.json"
            "--enable=warning,performance,style"
            "--error-exitcode=1"
            "--inline-suppr"
            "--suppress='*:/usr/*'"
            "--suppress='*:${CATCH_INCLUDE_DIRS}/*'"
            "--suppress='*:${GRAPHVIZ_INCLUDE_DIRS}/*'"
            "--suppress='*:${GTEST_INCLUDE_DIRS}/*'"
            "--suppress='*:${JSON_ROOT_DIRECTORY}/*'"
            "--suppress='*:${LLVM_INCLUDE_DIRS}/*'"
            "--suppress='*:${POLYMORPHIC_ROOT_DIR}/*'"
            "--suppress='*:${PYBIND11_INCLUDE_DIRS}/*'"
        )
endif()

# Set up google unit test target
add_executable(gtest_tests
        tests/visitor_test.cpp
        tests/shared_pointer_test.cpp
    )
target_link_libraries(gtest_tests
        runtime
        "${GTEST_LIBRARIES}"
    )
gtest_discover_tests(gtest_tests)
