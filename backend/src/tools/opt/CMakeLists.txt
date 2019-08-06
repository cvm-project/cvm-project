add_executable(opt-cli
        src/opt.cpp
    )

set_target_properties(opt-cli PROPERTIES
        OUTPUT_NAME "opt"
    )

target_link_libraries(opt-cli
    PUBLIC
        Boost::program_options
        dag
        generate
        print_dag
    )
