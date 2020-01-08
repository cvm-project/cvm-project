add_library(utils
        src/lib_path.cpp
    )

set_target_properties(utils PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )

target_include_directories(utils
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_link_libraries(utils
    PUBLIC
        Boost::boost
        Boost::filesystem
        Boost::system
    )

add_executable(utils_tests
        tests/visitor_test.cpp
    )
target_link_libraries(utils_tests
        googletest::gtest_main
        utils
    )
gtest_discover_tests(utils_tests)
