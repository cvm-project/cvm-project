#ifndef UTILS_C_TYPE_TO_LLVM_H
#define UTILS_C_TYPE_TO_LLVM_H

#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

llvm::Type *c_type_to_llvm(const std::string &type_,
                           llvm::LLVMContext *context);

#endif  // UTILS_C_TYPE_TO_LLVM_H