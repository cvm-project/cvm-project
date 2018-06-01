//
// Created by sabir on 18.07.17.
//

#include "LLVMParser.h"

#include <algorithm>
#include <regex>

#include <llvm/Transforms/Utils/Cloning.h>

#include "llvm_field_type_visitor.hpp"

void LLVMParser::parse(const std::string &ir) {
    module = parseIR(
            llvm::MemoryBufferRef(llvm::StringRef(ir), llvm::StringRef("id")),
            Err, Context);
    // find out the return type
    auto func = module->getFunctionList().begin();
    auto t = func->getReturnType();
    if (t->isAggregateType()) {
        ret_type = STRUCT;
        // save the insert instructions
        for (auto &b : *func) {
            for (auto i = b.begin(), ie = b.end(); i != ie; i++) {
                if (auto instr = llvm::cast<llvm::Instruction>(i)) {
                    if (instr->getOpcode() == llvm::Instruction::InsertValue) {
                        auto insert = llvm::cast<llvm::InsertValueInst>(instr);
                        assert(insert->getIndices().size() == 1);
                        const size_t pos = *(insert->idx_begin());
                        if (pos + 1 > ret_instruction_ids.size()) {
                            ret_instruction_ids.resize(pos + 1);
                        }
                        ret_instruction_ids[pos] = instr->getName();
                    }
                }
            }
        }
    } else if (t->isVoidTy()) {
        ret_type = CALLER_PTR;
        // save the store instructions
        for (auto &b : *func) {
            for (auto i = b.begin(), ie = b.end(); i != ie; ++i) {
                if (auto instr = llvm::cast<llvm::Instruction>(i)) {
                    if (instr->getOpcode() == llvm::Instruction::Store) {
                        // the store target uniquely identifies this instruction
                        ret_instruction_ids.push_back(
                                instr->getOperand(1)->getName());
                    }
                }
            }
        }
    } else {
        ret_type = PRIMITIVE;
    }
}

std::vector<size_t> LLVMParser::get_output_positions_primitive(
        size_t arg_position) {
    std::vector<size_t> res;

    auto function = module->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_position];
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;
        auto b = u.getUser();
        if (llvm::isa<llvm::Instruction>(b)) {
            auto inst = llvm::cast<llvm::Instruction>(b);
            if (llvm::Instruction::Ret == inst->getOpcode()) {
                res.push_back(0);
            }
            break;
        }
    }
    return res;
}

std::vector<size_t> LLVMParser::get_output_positions_struct(
        size_t arg_position) {
    std::vector<size_t> res;

    auto function = module->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_position];
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() == llvm::Instruction::InsertValue) {
                // find the index
                for (size_t i = 0; i < ret_instruction_ids.size(); i++) {
                    std::string id = ret_instruction_ids[i];
                    if (id == inst->getName()) {
                        res.push_back(i);
                    }
                }
            }
        }
    }
    return res;
}

std::vector<size_t> LLVMParser::get_output_positions_caller_ptr(
        size_t arg_position) {
    std::vector<size_t> res;

    // the first arg is the return pointer
    arg_position++;
    auto function = module->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_position];
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() == llvm::Instruction::Store) {
                // find the index
                for (size_t i = 0; i < ret_instruction_ids.size(); i++) {
                    std::string id = ret_instruction_ids[i];
                    if (id == inst->getOperand(1)->getName()) {
                        res.push_back(i);
                    }
                }
            }
        }
    }
    return res;
}

std::vector<size_t> LLVMParser::get_output_positions(size_t arg_position) {
    switch (ret_type) {
        case PRIMITIVE:
            return get_output_positions_primitive(arg_position);
        case STRUCT:
            return get_output_positions_struct(arg_position);
        case CALLER_PTR:
            return get_output_positions_caller_ptr(arg_position);
        default:
            return get_output_positions_primitive(arg_position);
    }
}

bool LLVMParser::is_argument_read(size_t arg_pos) {
    // go over uses and check if at least one is not an insert or a store
    if (ret_type == CALLER_PTR) {
        arg_pos++;
    }
    auto function = module->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_pos];

    bool used = false;
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() != llvm::Instruction::Store &&
                inst->getOpcode() != llvm::Instruction::InsertValue &&
                inst->getOpcode() != llvm::Instruction::Ret) {
                used = true;
                break;
            }
        } else {
            used = true;
            break;
        }
    }
    return used;
}

std::string LLVMParser::adjust_filter_signature(
        DAGFilter *const pFilter, const DAGOperator *const predecessor) {
    llvm::Module *new_mod = new llvm::Module("filter", Context);

    std::vector<llvm::Type *> types;
    for (const auto &f : predecessor->tuple->fields) {
        IR_analyzer::LLVMFieldTypeVisitor visitor(&Context);
        types.push_back(visitor.ComputeLLVMType(f.get()));
    }

    auto old_function = module->getFunctionList().begin();
    llvm::FunctionType *FT =
            llvm::FunctionType::get(llvm::Type::getInt1Ty(Context),
                                    llvm::ArrayRef<llvm::Type *>(types), false);
    llvm::Function *filter_predicate =
            llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                   old_function->getName(), new_mod);

    filter_predicate->getBasicBlockList().splice(
            filter_predicate->begin(), old_function->getBasicBlockList());

    size_t counter = 1;
    for (auto it = filter_predicate->arg_begin();
         it != filter_predicate->arg_end(); it++) {
        it->setName("." + std::to_string(counter++));
    }
    // In the IR, replace all fields with their new positions
    for (auto f : pFilter->tuple->fields) {
        if (!pFilter->Reads(f->attribute_id().get())) continue;
        const size_t old_pos = f->position();

        const auto input_field = std::find_if(
                predecessor->tuple->fields.begin(),
                predecessor->tuple->fields.end(), [&](auto const &input_f) {
                    return f->attribute_id() == input_f->attribute_id();
                });
        assert(input_field != predecessor->tuple->fields.end());
        const auto new_pos = input_field->get()->position();

        // replace all uses of oldPos argument to use of newPos argument
        auto old_arg = old_function->arg_begin() + old_pos;
        auto new_arg = filter_predicate->arg_begin() + new_pos;
        old_arg->replaceAllUsesWith(new_arg);
    }
    std::string ret;
    llvm::raw_string_ostream OS(ret);
    OS << *new_mod;
    OS.flush();
    //    new_mod->dump();
    delete (new_mod);
    return ret;
}
