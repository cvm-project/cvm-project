add_executable(runner
        src/main.cpp
    )

target_link_libraries(runner
    PUBLIC
        Boost::program_options
        dag
        runtime
        utils
        trace_exceptions
    )
