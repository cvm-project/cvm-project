pybind11_add_module(jitq_backend
        src/main.cpp
    )

target_link_libraries(jitq_backend
    PUBLIC
        pybind11::pybind11
    PRIVATE
        generate
        runtime
        trace_exceptions
    )
