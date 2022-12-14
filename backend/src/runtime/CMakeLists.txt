set(RUNTIME_SOURCE_FILES
        src/execute_plan.cpp
        src/fibers/trace.cpp
        src/fibers/asio/round_robin.cpp
        src/fibers/asio/yield.cpp
        src/filesystem/file.cpp
        src/filesystem/filesystem.cpp
        src/memory/shared_pointer.cpp
        src/memory/values.cpp
        src/net/tcp/exchange_service.cpp
        src/operators/arrow_helpers.cpp
        src/operators/arrow_table_scan.cpp
        src/operators/exchange_s3.cpp
        src/operators/exchange_tcp.cpp
        src/operators/expand_pattern.cpp
        src/operators/concurrent_execute_lambda.cpp
        src/operators/concurrent_execute_process.cpp
        src/operators/materialize_parquet_impl.cpp
        src/operators/murmur_hash2.cpp
        src/operators/parquet_scan_impl.cpp
        src/operators/record_batch_to_value.cpp
        src/operators/value_to_record_batch.cpp
        src/trace.cpp
        src/values/json_parsing.cpp
        src/values/array_json.cpp
        src/values/tuple_json.cpp
        src/values/value_json.cpp
        src/values/value_parser.cpp
    )

if(AWSSDK_FOUND)
    set(RUNTIME_SOURCE_FILES ${RUNTIME_SOURCE_FILES}
            src/aws/aggressive_retry_strategy.cpp
            src/aws/aws.cpp
            src/aws/http_client.cpp
            src/aws/s3.cpp
            src/aws/stderr_log_system.cpp
            src/filesystem/s3.cpp
            src/operators/exchange_s3.cpp
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
        dag
        utils
    PRIVATE
        Boost::boost
        Boost::fiber
        pybind11::embed
    )

add_executable(runtime_tests
        tests/shared_pointer_test.cpp
    )
target_link_libraries(runtime_tests
        googletest::gtest_main
        runtime
    )
gtest_discover_tests(runtime_tests
        DISCOVERY_TIMEOUT 10
    )
