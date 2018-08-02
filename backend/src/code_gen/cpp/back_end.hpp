#ifndef CODE_GEN_CPP_BACK_END_HPP
#define CODE_GEN_CPP_BACK_END_HPP

#include "code_gen/common/back_end.hpp"
#include "dag/DAG.h"

namespace code_gen {
namespace cpp {

class BackEnd : public common::BackEnd {
public:
    // cppcheck-suppress passedByValue
    explicit BackEnd(std::string config) : config_(std::move(config)) {}
    void Run(DAG* dag) const override;

private:
    const std::string config_;
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_BACK_END_HPP
