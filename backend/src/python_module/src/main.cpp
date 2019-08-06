#include <pybind11/pybind11.h>

#include "generate/generate_executable.hpp"
#include "runtime/execute_plan.hpp"
#include "runtime/memory/values.hpp"

// NOLINTNEXTLINE  This macro expands to things clang-tidy does not like,
// NOLINTNEXTLINE  but we do not have control over it.
PYBIND11_MODULE(jitq_backend, m) {
    m.doc() = R"pbdoc(
        JITQ
        ----

        .. currentmodule:: backend

        .. autosummary::
           :toctree: _generate
    )pbdoc";

    m.def("GenerateExecutable", &GenerateExecutable, R"pbdoc(
        Generate executable plan
    )pbdoc");

    m.def("ExecutePlan",
          static_cast<std::string (*)(size_t, const std::string&)>(
                  runtime::ExecutePlan),
          R"pbdoc(
        Execute previously generated plan
    )pbdoc");

    m.def("DumpDag", runtime::DumpDag,
          R"pbdoc(
        Dump previously generated plan as JSON string
    )pbdoc");

    m.def("FreeResult",
          static_cast<void (*)(const std::string&)>(runtime::memory::Decrement),
          R"pbdoc(
        Free memory of result
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
