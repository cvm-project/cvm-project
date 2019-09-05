option(WITH_TRACE_EXCEPTIONS_TO_STDERR
        "Trace stacktrace of thrown exceptions to stderr."
        OFF
    )

add_library(trace_exceptions SHARED
        src/cxa_throw.cpp
        src/patched_throw.cpp
        src/trace_exceptions.cpp
    )

target_include_directories(trace_exceptions
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_link_libraries(trace_exceptions
    PUBLIC
        Boost::stacktrace_basic
    PRIVATE
        Boost::boost
        dl
    )

if (${WITH_TRACE_EXCEPTIONS_TO_STDERR})
    target_compile_definitions(trace_exceptions
        PRIVATE
            "TRACE_EXCEPTIONS_TO_STDERR"
        )
endif()
