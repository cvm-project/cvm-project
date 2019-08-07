#ifndef RUNTIME_JIT_OPERATORS_EXPAND_PATTERN_HPP
#define RUNTIME_JIT_OPERATORS_EXPAND_PATTERN_HPP

#include <string>

namespace runtime {
namespace operators {

std::string ExpandPattern(const std::string &pattern, int64_t parameter);

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_EXPAND_PATTERN_HPP
