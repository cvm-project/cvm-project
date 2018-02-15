#ifndef CODE_GEN_CPP_GENERATE_CODE_H
#define CODE_GEN_CPP_GENERATE_CODE_H

//
// Created by sabir on 19.07.17.
//

#include "CodeGenVisitor.h"
#include "dag/DAG.h"

namespace code_gen {
namespace cpp {

void generate_code(DAG* dag);

}  // namespace cpp
}  // namespace code_gen

#endif
