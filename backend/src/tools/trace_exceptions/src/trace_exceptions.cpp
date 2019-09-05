#include "trace_exceptions/trace_exceptions.hpp"

#include <memory>
#include <string>

#ifdef __GLIBCXX__
#include <cxxabi.h>
#endif  // __GLIBCXX__

namespace trace_exceptions {
namespace impl {

thread_local ExceptionInfo current_exception_info;

}  // namespace impl

std::string demangle(const char *const symbol) {
#ifdef __GLIBCXX__
    int status;
    std::unique_ptr<char, void (*)(void *)> demangled_symbol(
            abi::__cxa_demangle(symbol, nullptr, nullptr, &status), &std::free);
    if (status != 0) return demangled_symbol.get();
#endif  // __GLIBCXX__
    return symbol;
}

}  // namespace trace_exceptions
