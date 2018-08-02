#ifndef CODE_GEN_COMMON_BACK_END_HPP
#define CODE_GEN_COMMON_BACK_END_HPP

#include <cstdint>
#include <string>

class DAG;

namespace code_gen {
namespace common {

class BackEnd {
public:
    virtual ~BackEnd() = default;
    virtual void Run(DAG* dag) const = 0;
};

}  // namespace common
}  // namespace code_gen

#endif  // CODE_GEN_COMMON_BACK_END_HPP
