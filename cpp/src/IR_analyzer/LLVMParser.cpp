//
// Created by sabir on 18.07.17.
//

#include "LLVMParser.h"
#include <regex>
#include "utils/c_type_to_llvm.h"
#include <llvm/Transforms/Utils/Cloning.h>


void LLVMParser::parse(std::string &ir) {
    module = parseIR(MemoryBufferRef(StringRef(ir), StringRef("id")), Err, Context);
    //find out the return type
    auto func = module->getFunctionList().begin();
    auto t = func->getReturnType();
    if (t->isAggregateType()) {
        ret_type = STRUCT;
        //save the insert instructions
        for (auto it = func->begin(); it != func->end(); it++) {
            BasicBlock &b = *it;
            for (auto i = b.begin(), ie = b.end(); i != ie; i++) {
                if (auto instr = cast<Instruction>(i)) {
                    if (instr->getOpcode() == Instruction::InsertValue) {
                        ret_instruction_ids.push_back(instr->getName());
                    }
                }
            }

        }
    }
    else if (t->isVoidTy()) {
        ret_type = CALLER_PTR;
        //save the store instructions
        for (auto it = func->begin(); it != func->end(); it++) {
            BasicBlock &b = *it;
            for (auto i = b.begin(), ie = b.end(); i != ie; ++i) {
                if (auto instr = cast<Instruction>(i)) {
                    if (instr->getOpcode() == Instruction::Store) {
                        //the store target uniquely identifies this instruction
                        ret_instruction_ids.push_back(instr->getOperand(1)->getName());
                    }
                }
            }

        }
    }
    else {
        ret_type = PRIMITIVE;
    }
}


std::vector<size_t> LLVMParser::get_output_positions_primitive(size_t arg_position) {
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


std::vector<size_t> LLVMParser::get_output_positions_struct(size_t arg_position) {
    std::vector<size_t> res;

    auto function = module->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_position];
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() == Instruction::InsertValue) {
                //find the index
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


std::vector<size_t> LLVMParser::get_output_positions_caller_ptr(size_t arg_position) {
    std::vector<size_t> res;

    //the first arg is the return pointer
    arg_position++;
    auto function = module->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_position];
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() == Instruction::Store) {
                //find the index
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
    //go over uses and check if at least one is not an insert or a store
    if (ret_type == CALLER_PTR) {
        arg_pos++;
    }
    auto function = module->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_pos];

    bool used = 0;
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() != Instruction::Store && inst->getOpcode() != Instruction::InsertValue &&
                inst->getOpcode() != Instruction::Ret) {
                used = 1;
                break;
            }
        }
        else {
            used = 1;
            break;
        }
    }
    return used;
}

std::string LLVMParser::adjust_filter_signature(DAGFilter *pFilter) {

    DAGOperator *predecessor = pFilter->predecessors[0];
    Module *new_mod = new Module("filter", Context);


    std::vector<Type *> types;
    for (auto f : predecessor->fields) {
        types.push_back(c_type_to_llvm(f.type, Context));
    }

    auto old_function = module->getFunctionList().begin();
    FunctionType *FT =
            FunctionType::get(Type::getInt1Ty(Context), ArrayRef<Type *>(types), false);
    Function *filter_predicate =
            Function::Create(FT, Function::ExternalLinkage, old_function->getName(), new_mod);

    filter_predicate->getBasicBlockList().splice(filter_predicate->begin(), old_function->getBasicBlockList());

    size_t counter = 1;
    for (auto it = filter_predicate->arg_begin(); it != filter_predicate->arg_end(); it++) {
        it->setName("." + to_string(counter++));
    }
    //go over the read set
    for (auto c : pFilter->read_set) {
        size_t oldPos = 0;
        for (auto f : pFilter->fields) {
            if (*f.column == *c) {
                oldPos = f.position;
                break;
            }
        }
        size_t newPos = 0;
        for (auto f : predecessor->fields) {
            if (*f.column == *c) {
                newPos = f.position;
                break;
            }
        }
        //replace all uses of oldPos argument to use of newPos argument
        auto oldArg = old_function->arg_begin()+oldPos;
        auto newArg = filter_predicate->arg_begin()+newPos;
        oldArg->replaceAllUsesWith(newArg);
    }
    string ret;
    raw_string_ostream OS(ret);
    OS << *new_mod;
    OS.flush();
//    new_mod->dump();
    delete (new_mod);
    return ret;
}