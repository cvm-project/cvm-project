pybind11_add_module(jitq_backend
        src/main.cpp
    )

target_link_libraries(jitq_backend
    PRIVATE
        generate
        runtime
    )


