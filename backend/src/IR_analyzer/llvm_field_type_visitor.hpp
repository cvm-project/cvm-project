//
// Created by sabir on 16.05.18.
//

#ifndef CPP_LLVM_FIELD_TYPE_VISITOR_HPP
#define CPP_LLVM_FIELD_TYPE_VISITOR_HPP

#include "dag/utils/field_visitor.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

namespace IR_analyzer {

class LLVMFieldTypeVisitor : public dag::utils::FieldVisitor {
public:
    explicit LLVMFieldTypeVisitor(llvm::LLVMContext *context)
        : context(context) {}

    llvm::Type *ComputeLLVMType(dag::collection::Field *field);

protected:
    void Visit(dag::collection::Atomic *field) override;
    void Visit(dag::collection::Array *field) override;

private:
    llvm::LLVMContext *context;
    llvm::Type *ret_llvm_type{};
};

}  // namespace IR_analyzer
#endif  // CPP_LLVM_FIELD_TYPE_VISITOR_HPP
