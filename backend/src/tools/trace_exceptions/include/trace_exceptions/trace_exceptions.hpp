#ifndef TRACE_EXCEPTIONS_TRACE_EXCEPTIONS_HPP
#define TRACE_EXCEPTIONS_TRACE_EXCEPTIONS_HPP

#include <optional>
#include <string>

#include <boost/stacktrace.hpp>

namespace trace_exceptions {
namespace impl {

struct ExceptionInfo {
    void *ptr{};
    const std::type_info *type{};
    boost::stacktrace::stacktrace stacktrace;
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern thread_local ExceptionInfo current_exception_info;

}  //  namespace impl

template <class ExceptionType>
auto GetStackTrace(const ExceptionType &ex)
        -> std::optional<boost::stacktrace::stacktrace> {
    auto const current_info = impl::current_exception_info;
    if (&typeid(ex) != current_info.type || current_info.ptr != &ex) {
        return {};
    }
    return current_info.stacktrace;
}

auto demangle(const char *symbol) -> std::string;

}  // namespace trace_exceptions

#endif  // TRACE_EXCEPTIONS_TRACE_EXCEPTIONS_HPP
