add_library(print_dag SHARED
        src/print_dag.cpp
    )

target_include_directories(print_dag
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_link_libraries(print_dag
    PUBLIC
        dag
    PRIVATE
        Boost::boost
        GraphViz::gvc
    )
