#ifndef RUNTIME_JIT_OPERATORS_MURMUR_HASH2_HPP
#define RUNTIME_JIT_OPERATORS_MURMUR_HASH2_HPP

namespace runtime {
namespace operators {
namespace MurmurHash2 {

auto Hash(const void *key, unsigned int len, unsigned int seed) -> unsigned int;

}  // namespace MurmurHash2
}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_JIT_OPERATORS_MURMUR_HASH2_HPP
