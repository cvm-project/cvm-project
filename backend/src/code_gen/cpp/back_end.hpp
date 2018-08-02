#ifndef CODE_GEN_CPP_BACK_END_HPP
#define CODE_GEN_CPP_BACK_END_HPP

#include "code_gen/common/back_end.hpp"
#include "dag/DAG.h"

namespace code_gen {
namespace cpp {

class BackEnd : public common::BackEnd {
public:
    void GenerateCode(DAG *dag) final;
    void Compile(uint64_t counter) final;

private:
    boost::filesystem::path lib_dir_;
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_BACK_END_HPP
