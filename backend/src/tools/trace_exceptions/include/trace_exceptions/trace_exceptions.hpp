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

extern thread_local ExceptionInfo current_exception_info;

}  //  namespace impl

template <class ExceptionType>
std::optional<boost::stacktrace::stacktrace> GetStackTrace(
        const ExceptionType &ex) {
    auto const current_info = impl::current_exception_info;
    if (&typeid(ex) != current_info.type || current_info.ptr != &ex) {
        return {};
    }
    return current_info.stacktrace;
}

std::string demangle(const char *symbol);

}  // namespace trace_exceptions

#endif  // TRACE_EXCEPTIONS_TRACE_EXCEPTIONS_HPP
