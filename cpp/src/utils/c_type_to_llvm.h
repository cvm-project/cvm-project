//
// Created by sabir on 29.08.17.
//

#include <string>

#include <assert.h>

#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

using namespace llvm;

Type *c_type_to_llvm(const std::string type_, LLVMContext *context) {
    assert(context != nullptr);
    if (type_ == "long") return Type::getInt64Ty(*context);
    if (type_ == "int") return Type::getInt32Ty(*context);
    if (type_ == "bool") return Type::getInt1Ty(*context);
    if (type_ == "float") return Type::getFloatTy(*context);
    if (type_ == "double") return Type::getDoubleTy(*context);
    return Type::getDoubleTy(*context);
}
