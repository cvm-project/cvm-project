#ifndef CODE_GEN_COMMON_BACK_END_HPP
#define CODE_GEN_COMMON_BACK_END_HPP

#include <cstdint>

class DAG;

namespace code_gen {
namespace common {

struct BackEnd {
    virtual void GenerateCode(DAG* dag) = 0;
    virtual void Compile(uint64_t counter) = 0;
};

}  // namespace common
}  // namespace code_gen

#endif  // CODE_GEN_COMMON_BACK_END_HPP
