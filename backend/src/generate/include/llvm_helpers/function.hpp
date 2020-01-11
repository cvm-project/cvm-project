//
// Created by sabir on 18.07.17.
//

#ifndef LLVM_HELPERS_FUNCTION_HPP
#define LLVM_HELPERS_FUNCTION_HPP

#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "dag/operators/filter.hpp"

namespace llvm_helpers {

class Function {
public:
    explicit Function(const std::string &ir);

    /**
     * if the argument is part of the output, return its positions in the output
     */
    auto ComputeOutputPositions(size_t arg_position) const
            -> std::vector<size_t>;

    /**
     * whether this argument is used by the function to produce any output
     *
     */
    auto ComputeIsArgumentRead(size_t arg_pos) const -> bool;

    auto AdjustFilterSignature(DAGFilter *pFilter,
                               const DAGOperator *predecessor) -> std::string;
    auto AddInlineAttribute() -> std::string;

private:
    /**
     * we need to differentiate between 3 cases
     * 1) the function has only one return value, e.g. ret i64 %.1
     * 2) the function returns a tuple
     *
     *            %.16.fca.0.insert = insertvalue { i64, i64 } undef, i64 %.1, 0
                  %.16.fca.1.insert = insertvalue { i64, i64 }
     %.16.fca.0.insert, i64 3, 1
                  ret { i64, i64 } %.16.fca.1.insert

      *3) the function writes to the return pointer passed by the caller
      *
          %.1.repack5 = bitcast { i64, i64, i64 }* %.1 to i64*
          store i64 %.2, i64* %.1.repack5, align 8
          %.1.repack1 = getelementptr inbounds { i64, i64, i64 }, { i64, i64,
     i64 }* %.1, i64 0, i32 1
          store i64 2, i64* %.1.repack1, align 8
          %.1.repack3 = getelementptr inbounds { i64, i64, i64 }, { i64, i64,
     i64 }* %.1, i64 0, i32 2
          store i64 3, i64* %.1.repack3, align 8
          ret void
     */
    enum class ReturnType { kUnknown, kPrimitive, kStruct, kCallerPtr };

    auto ComputeOutputPositionsPrimitive(size_t arg_position) const
            -> std::vector<size_t>;
    auto ComputeOutputPositionsStruct(size_t arg_position) const
            -> std::vector<size_t>;
    auto ComputeOutputPositionsCallerPtr(size_t arg_position) const
            -> std::vector<size_t>;

    // these 3 fields must be declared in this order
    llvm::SMDiagnostic err_;
    llvm::LLVMContext context_;
    std::unique_ptr<llvm::Module> module_;

    std::vector<std::string> ret_instruction_ids_;
    ReturnType ret_type_;
};

}  // namespace llvm_helpers

#endif  // LLVM_HELPERS_FUNCTION_HPP
