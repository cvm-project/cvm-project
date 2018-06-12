#ifndef CODE_GEN_CPP_BACKEND_HPP
#define CODE_GEN_CPP_BACKEND_HPP

#include "code_gen/common/BackEnd.hpp"
#include "dag/DAG.h"

namespace code_gen {
namespace cpp {

class BackEnd : public common::BackEnd {
public:
    void GenerateCode(DAG *dag) final;
    void Compile(uint64_t counter) final;
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_BACKEND_HPP
