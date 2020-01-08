set(RUNTIME_SOURCE_FILES
        src/execute_plan.cpp
        src/filesystem/file.cpp
        src/filesystem/filesystem.cpp
        src/memory/shared_pointer.cpp
        src/memory/values.cpp
        src/operators/arrow_helpers.cpp
        src/operators/expand_pattern.cpp
        src/operators/materialize_parquet_impl.cpp
        src/operators/parquet_scan_impl.cpp
        src/operators/value_to_record_batch.cpp
        src/values/json_parsing.cpp
        src/values/array_json.cpp
        src/values/tuple_json.cpp
        src/values/value_json.cpp
        src/values/value_parser.cpp
    )

if(AWSSDK_FOUND)
    set(RUNTIME_SOURCE_FILES ${RUNTIME_SOURCE_FILES}
            src/aws/aws.cpp
            src/aws/s3.cpp
            src/filesystem/s3.cpp
        )
endif()

add_library(runtime SHARED ${RUNTIME_SOURCE_FILES})

target_include_directories(runtime
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_link_libraries(runtime
    PUBLIC
        nlohmann_json::nlohmann_json
        Skyr::skyr-url
        ${AWSSDK_LINK_LIBRARIES}
        ${arrow_LIBRARY}
        ${parquet_LIBRARY}
        utils
    PRIVATE
        Boost::boost
    )

add_executable(runtime_tests
        tests/shared_pointer_test.cpp
    )
target_link_libraries(runtime_tests
        googletest::gtest_main
        runtime
    )
gtest_discover_tests(runtime_tests)
