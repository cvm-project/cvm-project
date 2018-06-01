//
// Created by sabir on 16.05.18.
//

#include "llvm_field_type_visitor.hpp"
#include "utils/c_type_to_llvm.h"

#include "dag/collection/array.hpp"
#include "dag/collection/atomic.hpp"

llvm::Type *IR_analyzer::LLVMFieldTypeVisitor::ComputeLLVMType(
        dag::collection::Field *field) {
    field->Accept(this);
    return ret_llvm_type;
}

void IR_analyzer::LLVMFieldTypeVisitor::Visit(dag::collection::Atomic *field) {
    ret_llvm_type = c_type_to_llvm(field->field_type()->type, context);
}
void IR_analyzer::LLVMFieldTypeVisitor::Visit(
        dag::collection::Array * /*collection*/) {
    throw "Accepting arrays in llvm IR not implemented yet.";
}
