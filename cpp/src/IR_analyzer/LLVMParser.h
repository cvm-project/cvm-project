//
// Created by sabir on 18.07.17.
//

#ifndef CPP_PARSER_H
#define CPP_PARSER_H

#include <memory>
#include <vector>
#include <string>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <dag/DAGFilter.h>


using namespace llvm;

class LLVMParser {
public:

    LLVMParser(std::string &ir){
        parse(ir);
    }

    void parse(std::string&);

    /**
     * if the argument is part of the output, return its positions in the output
     */
    std::vector<size_t> get_output_positions(size_t arg_pos);

    /**
     * whether this argument is used by the function to produce any output
     *
     */
    bool is_argument_read(size_t arg_pos);

    std::string adjust_filter_signature(DAGFilter *pFilter);

private:
    //these 3 fields must be declared in this order
    SMDiagnostic Err;
    LLVMContext Context;
    std::unique_ptr<Module> module;
    std::vector<std::string> ret_instruction_ids;
    std::vector<size_t> get_output_positions_primitive(size_t arg_pos);
    std::vector<size_t> get_output_positions_struct(size_t arg_pos);
    std::vector<size_t> get_output_positions_caller_ptr(size_t arg_pos);



    /**
     * we need to differentiate between 3 cases
     * 1) the function has only one return value, e.g. ret i64 %.1
     * 2) the function returns a tuple
     *
     *            %.16.fca.0.insert = insertvalue { i64, i64 } undef, i64 %.1, 0
                  %.16.fca.1.insert = insertvalue { i64, i64 } %.16.fca.0.insert, i64 3, 1
                  ret { i64, i64 } %.16.fca.1.insert

      *3) the function writes to the return pointer passed by the caller
      *
          %.1.repack5 = bitcast { i64, i64, i64 }* %.1 to i64*
          store i64 %.2, i64* %.1.repack5, align 8
          %.1.repack1 = getelementptr inbounds { i64, i64, i64 }, { i64, i64, i64 }* %.1, i64 0, i32 1
          store i64 2, i64* %.1.repack1, align 8
          %.1.repack3 = getelementptr inbounds { i64, i64, i64 }, { i64, i64, i64 }* %.1, i64 0, i32 2
          store i64 3, i64* %.1.repack3, align 8
          ret void
     */
    enum ret_types {PRIMITIVE, STRUCT, CALLER_PTR};
    ret_types ret_type;
};


#endif //CPP_PARSER_H