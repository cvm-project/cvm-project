#ifndef CODE_GEN_CPP_BACK_END_HPP
#define CODE_GEN_CPP_BACK_END_HPP

#include <string>

#include "dag/DAG.h"

namespace code_gen {
namespace cpp {

class BackEnd {
public:
    // cppcheck-suppress passedByValue
    explicit BackEnd(std::string config) : config_(std::move(config)) {}
    std::string Run(DAG* dag) const;

private:
    const std::string config_;
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_BACK_END_HPP
