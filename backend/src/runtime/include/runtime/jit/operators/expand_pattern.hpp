#ifndef RUNTIME_JIT_OPERATORS_EXPAND_PATTERN_HPP
#define RUNTIME_JIT_OPERATORS_EXPAND_PATTERN_HPP

#include <string>

namespace runtime {
namespace operators {

auto ExpandPattern(const std::string &pattern, int64_t parameter)
        -> std::string;

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_EXPAND_PATTERN_HPP
