#include "runtime/jit/operators/expand_pattern.hpp"

#include <boost/format.hpp>

namespace runtime::operators {

auto ExpandPattern(const std::string &pattern, const int64_t parameter)
        -> std::string {
    boost::format formatter(pattern);
    formatter.exceptions(boost::io::all_error_bits ^
                         boost::io::too_many_args_bit);
    return (formatter % parameter).str();
}

}  // namespace runtime::operators
