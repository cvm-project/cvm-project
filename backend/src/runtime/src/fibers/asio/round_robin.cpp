#include "round_robin.hpp"

namespace boost::fibers::asio {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)
boost::asio::io_service::id round_robin::service::id;

}  // namespace boost::fibers::asio
