#include "patched_throw.hpp"

#ifdef TRACE_EXCEPTIONS_TO_STDERR
#include <iostream>
#endif  // TRACE_EXCEPTIONS_TO_STDERR

#include <dlfcn.h>

#include <boost/stacktrace.hpp>

#include "trace_exceptions/trace_exceptions.hpp"

namespace trace_exceptions {

void StoreInfoAndThrow(void *ex, void *info, void (*dest)(void *)) {
#ifdef TRACE_EXCEPTIONS_TO_STDERR
    const auto type = reinterpret_cast<const std::type_info *>(info);
    impl::current_exception_info =
            impl::ExceptionInfo{ex, type, boost::stacktrace::stacktrace()};

    std::cerr << "Exception thrown of type "
              << (type != nullptr ? demangle(type->name()) : "(unknown)")
              << " at:" << std::endl;
    std::cerr << impl::current_exception_info.stacktrace << std::endl;
#endif  // TRACE_EXCEPTIONS_TO_STDERR

    using ThrowType = void (*const)(void *, void *, void (*)(void *));

    const auto static rethrow =
            reinterpret_cast<ThrowType>(dlsym(RTLD_NEXT, "__cxa_throw"));

    rethrow(ex, info, dest);
}

}  // namespace trace_exceptions
