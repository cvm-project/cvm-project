#include "yield.hpp"

namespace boost::fibers::asio {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
thread_local yield_t yield{};

}  // namespace boost::fibers::asio
