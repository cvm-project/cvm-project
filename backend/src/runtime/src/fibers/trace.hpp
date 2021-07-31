#ifndef FIBERS_TRACE_HPP
#define FIBERS_TRACE_HPP

#include <iostream>
#include <sstream>
#include <string>

namespace runtime::fibers {

auto this_thread_name() -> std::string;
auto this_fiber_name() -> std::string;
auto tag() -> std::string;

namespace detail {

void print_impl(std::ostream &out);

template <typename T, typename... Ts>
void print_impl(std::ostream &out, T const &arg, Ts const &...args) {
    out << arg;
    detail::print_impl(out, args...);
}

auto wall_time() -> std::string;

}  // namespace detail

template <typename... T>
void print(T const &...args) {
    std::ostringstream buffer;
    detail::print_impl(buffer, detail::wall_time(), ": ", args...);
    std::cerr << buffer.str() << std::flush;
}

}  // namespace runtime::fibers

#endif  // FIBERS_TRACE_HPP
