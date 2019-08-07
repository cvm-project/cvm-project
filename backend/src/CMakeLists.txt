cmake_minimum_required(VERSION 3.8)
project(cpp)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/cmake/")

set(LLVM_ASAN OFF CACHE BOOL "Enable compilation with LLVM address, undefined, and leak sanitizer.")

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

set(_LLVM_LIBS "LLVMCore;LLVMIRReader;LLVMSupport")
foreach(lib ${_LLVM_LIBS})
    target_include_directories(${lib} INTERFACE ${LLVM_INCLUDE_DIRS})
endforeach()

# Set up GraphViz
find_package(GraphViz REQUIRED)

# Set up Catch (for unit tests)
set(CATCH_ROOT_DIR "${PROJECT_SOURCE_DIR}/../../3rdparty/catch")
get_filename_component(CATCH_ROOT_DIR "${CATCH_ROOT_DIR}" ABSOLUTE)
add_subdirectory("${CATCH_ROOT_DIR}" catch2)

# Set up JSON library
set(JSON_ROOT_DIRECTORY "${PROJECT_SOURCE_DIR}/../../3rdparty/json/")
get_filename_component(JSON_ROOT_DIRECTORY "${JSON_ROOT_DIRECTORY}" ABSOLUTE)
set(JSON_BuildTests OFF CACHE BOOL
    "Build the unit tests when BUILD_TESTING is enabled.")
add_subdirectory("${JSON_ROOT_DIRECTORY}" nlohmann_json)

# Set up pybind11
set(PYBIND11_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/pybind11")
get_filename_component(PYBIND11_ROOT_DIR "${PYBIND11_ROOT_DIR}" ABSOLUTE)
add_subdirectory("${PYBIND11_ROOT_DIR}" pybind11)

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
set(_BOOST_COMPILE_DEFINITIONS
        "BOOST_MPL_LIMIT_LIST_SIZE=30"
        "BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS"
    )
set_target_properties(Boost::boost PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "${_BOOST_COMPILE_DEFINITIONS}"
    )

# Set up skyr URL library
set(Skyr_ROOT_DIR "${PROJECT_SOURCE_DIR}/../../3rdparty/skyr-url")
get_filename_component(Skyr_ROOT_DIR "${Skyr_ROOT_DIR}" ABSOLUTE)
set(Skyr_BUILD_TESTS OFF CACHE BOOL "")
set(Skyr_DISABLE_LIBCXX ON CACHE BOOL "")
set(Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS OFF CACHE BOOL "")
add_subdirectory("${Skyr_ROOT_DIR}" skyr-url)

# Set up google unit test framework
set(GTEST_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/googletest)
get_filename_component(GTEST_ROOT_DIR "${GTEST_ROOT_DIR}" ABSOLUTE)
add_subdirectory(${GTEST_ROOT_DIR} googletest)
add_library(googletest::gtest ALIAS gtest)
add_library(googletest::gtest_main ALIAS gtest_main)

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

add_subdirectory(benchmarks)
add_subdirectory(dag)
add_subdirectory(generate)
add_subdirectory(python_module)
add_subdirectory(runtime)
add_subdirectory(tools)
add_subdirectory(utils)

# Setup cppcheck
find_package(cppcheck 1.88 EXACT)
if(cppcheck_FOUND)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(
        cppcheck
        COMMAND ${cppcheck_EXECUTABLE}
            "--project=compile_commands.json"
            "--enable=warning,performance,style"
            "--error-exitcode=1"
            "--inline-suppr"
            "--suppress=useStlAlgorithm"
            "--suppress='*:/usr/*'"
            "--suppress='*:${CATCH_ROOT_DIR}/*'"
            "--suppress='*:${GRAPHVIZ_INCLUDE_DIRS}/*'"
            "--suppress='*:${GTEST_ROOT_DIR}/*'"
            "--suppress='*:${JSON_ROOT_DIRECTORY}/*'"
            "--suppress='*:${LLVM_INCLUDE_DIRS}/*'"
            "--suppress='*:${POLYMORPHIC_ROOT_DIR}/*'"
            "--suppress='*:${PYBIND11_ROOT_DIR}/*'"
            "--suppress='*:${Skyr_ROOT_DIR}/*'"
        )
endif()
