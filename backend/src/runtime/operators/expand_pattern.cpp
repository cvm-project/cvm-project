#include "expand_pattern.hpp"

#include <boost/format.hpp>

namespace runtime {
namespace operators {

std::string ExpandPattern(const std::string &pattern, const int64_t parameter) {
    boost::format formatter(pattern);
    formatter.exceptions(boost::io::all_error_bits ^
                         boost::io::too_many_args_bit);
    return (formatter % parameter).str();
}

}  // namespace operators
}  // namespace runtime
