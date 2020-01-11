#include "llvm_helpers/function.hpp"

#include <algorithm>
#include <regex>

#include <boost/mpl/list.hpp>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "dag/collection/atomic.hpp"
#include "dag/collection/field.hpp"
#include "utils/visitor.hpp"

namespace llvm_helpers {

auto c_type_to_llvm(const std::string &type_, llvm::LLVMContext *context)
        -> llvm::Type * {
    assert(context != nullptr);
    if (type_ == "long") return llvm::Type::getInt64Ty(*context);
    if (type_ == "int") return llvm::Type::getInt32Ty(*context);
    if (type_ == "bool") return llvm::Type::getInt1Ty(*context);
    if (type_ == "float") return llvm::Type::getFloatTy(*context);
    if (type_ == "double") return llvm::Type::getDoubleTy(*context);
    return llvm::Type::getDoubleTy(*context);
}

auto ComputeLLVMType(llvm::LLVMContext *const context,
                     dag::collection::Field *const field) -> llvm::Type * {
    class LLVMFieldTypeVisitor
        : public Visitor<LLVMFieldTypeVisitor, dag::collection::Field,
                         boost::mpl::list<                //
                                 dag::collection::Atomic  //
                                 >::type,
                         llvm::Type *> {
    public:
        explicit LLVMFieldTypeVisitor(llvm::LLVMContext *context)
            : context(context) {}

        auto operator()(dag::collection::Atomic *const field) -> llvm::Type * {
            return c_type_to_llvm(field->field_type()->type, context);
        }

    private:
        llvm::LLVMContext *context;
    };

    return LLVMFieldTypeVisitor(context).Visit(field);
}

Function::Function(const std::string &ir) : ret_type_(ReturnType::kUnknown) {
    module_ = parseIR(
            llvm::MemoryBufferRef(llvm::StringRef(ir), llvm::StringRef("id")),
            err_, context_);
    // find out the return type
    auto func = module_->getFunctionList().begin();
    auto t = func->getReturnType();
    if (t->isAggregateType()) {
        ret_type_ = ReturnType::kStruct;
        // save the insert instructions
        for (auto &b : *func) {
            for (auto i = b.begin(), ie = b.end(); i != ie; i++) {
                if (auto instr = llvm::cast<llvm::Instruction>(i)) {
                    if (instr->getOpcode() == llvm::Instruction::InsertValue) {
                        auto insert = llvm::cast<llvm::InsertValueInst>(instr);
                        assert(insert->getIndices().size() == 1);
                        const size_t pos = *(insert->idx_begin());
                        if (pos + 1 > ret_instruction_ids_.size()) {
                            ret_instruction_ids_.resize(pos + 1);
                        }
                        ret_instruction_ids_[pos] = instr->getName();
                    }
                }
            }
        }
    } else if (t->isVoidTy()) {
        ret_type_ = ReturnType::kCallerPtr;
        // save the store instructions
        for (auto &b : *func) {
            for (auto i = b.begin(), ie = b.end(); i != ie; ++i) {
                if (auto instr = llvm::cast<llvm::Instruction>(i)) {
                    if (instr->getOpcode() == llvm::Instruction::Store) {
                        // the store target uniquely identifies this instruction
                        ret_instruction_ids_.push_back(
                                instr->getOperand(1)->getName());
                    }
                }
            }
        }
    } else {
        ret_type_ = ReturnType::kPrimitive;
    }
}

auto Function::ComputeOutputPositionsPrimitive(size_t arg_position) const
        -> std::vector<size_t> {
    std::vector<size_t> res;

    auto function = module_->getFunctionList().begin();
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

auto Function::ComputeOutputPositionsStruct(size_t arg_position) const
        -> std::vector<size_t> {
    std::vector<size_t> res;

    auto function = module_->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_position];
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() == llvm::Instruction::InsertValue) {
                // find the index
                for (size_t i = 0; i < ret_instruction_ids_.size(); i++) {
                    std::string id = ret_instruction_ids_[i];
                    if (id == inst->getName()) {
                        res.push_back(i);
                    }
                }
            }
        }
    }
    return res;
}

auto Function::ComputeOutputPositionsCallerPtr(size_t arg_position) const
        -> std::vector<size_t> {
    std::vector<size_t> res;

    // the first arg is the return pointer
    arg_position++;
    auto function = module_->getFunctionList().begin();
    auto &s = function->arg_begin()[arg_position];
    for (auto it = s.use_begin(); it != s.use_end(); it++) {
        auto &u = *it;

        if (auto inst = llvm::cast<llvm::Instruction>(u.getUser())) {
            if (inst->getOpcode() == llvm::Instruction::Store) {
                // find the index
                for (size_t i = 0; i < ret_instruction_ids_.size(); i++) {
                    std::string id = ret_instruction_ids_[i];
                    if (id == inst->getOperand(1)->getName()) {
                        res.push_back(i);
                    }
                }
            }
        }
    }
    return res;
}

auto Function::ComputeOutputPositions(size_t arg_position) const
        -> std::vector<size_t> {
    switch (ret_type_) {
        case ReturnType::kPrimitive:
            return ComputeOutputPositionsPrimitive(arg_position);
        case ReturnType::kStruct:
            return ComputeOutputPositionsStruct(arg_position);
        case ReturnType::kCallerPtr:
            return ComputeOutputPositionsCallerPtr(arg_position);
        default:
            return ComputeOutputPositionsPrimitive(arg_position);
    }
}

auto Function::ComputeIsArgumentRead(size_t arg_pos) const -> bool {
    // go over uses and check if at least one is not an insert or a store
    if (ret_type_ == ReturnType::kCallerPtr) {
        arg_pos++;
    }
    auto function = module_->getFunctionList().begin();
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

auto Function::AdjustFilterSignature(DAGFilter *const pFilter,
                                     const DAGOperator *const predecessor)
        -> std::string {
    auto const new_mod = std::make_unique<llvm::Module>("filter", context_);

    std::vector<llvm::Type *> types;
    for (const auto &f : predecessor->tuple->fields) {
        types.push_back(ComputeLLVMType(&context_, f.get()));
    }

    // add the return pointer type to the argument types
    types.insert(types.begin(), llvm::Type::getInt8PtrTy(context_));

    auto old_function = module_->getFunctionList().begin();
    llvm::FunctionType *FT =
            llvm::FunctionType::get(llvm::Type::getVoidTy(context_),
                                    llvm::ArrayRef<llvm::Type *>(types), false);
    llvm::Function *filter_predicate =
            llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                   old_function->getName(), new_mod.get());

    filter_predicate->getBasicBlockList().splice(
            filter_predicate->begin(), old_function->getBasicBlockList());

    size_t counter = 1;
    for (auto it = filter_predicate->arg_begin();
         it != filter_predicate->arg_end(); it++) {
        it->setName("." + std::to_string(counter++));
    }
    // In the IR, replace all fields with their new positions
    for (const auto &f : pFilter->tuple->fields) {
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
        // +1 accounts for the return pointer
        auto old_arg = old_function->arg_begin() + old_pos + 1;
        auto new_arg = filter_predicate->arg_begin() + new_pos + 1;
        old_arg->replaceAllUsesWith(new_arg);
    }
    // return pointer
    auto old_arg = old_function->arg_begin();
    auto new_arg = filter_predicate->arg_begin();
    old_arg->replaceAllUsesWith(new_arg);
    std::string ret;
    llvm::raw_string_ostream OS(ret);
    OS << *new_mod;
    OS.flush();
    return ret;
}

auto Function::AddInlineAttribute() -> std::string {
    auto function = module_->getFunctionList().begin();
    function->addFnAttr(llvm::Attribute::AlwaysInline);
    std::string ret;
    llvm::raw_string_ostream OS(ret);
    OS << *module_;
    OS.flush();
    return ret;
}

}  // namespace llvm_helpers
