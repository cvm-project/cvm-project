//
// Created by sabir on 18.07.17.
//

#include "Parser.h"

void Parser::parse() {
    const char *ir = "define { i64, double } @\"cfunc._ZN8__main__16$3clambda$3e$242Ex\"(i64 %.1) local_unnamed_addr #1 {\nentry:\n  %.14.i = sitofp i64 %.1 to double\n  %.15.i = fadd double %.14.i, 1.000000e+00\n  %.15.fca.0.insert = insertvalue { i64, double } undef, i64 %.1, 0\n  %.15.fca.1.insert = insertvalue { i64, double } %.15.fca.0.insert, double %.15.i, 1\n  ret { i64, double } %.15.fca.1.insert\n}\n\n";

    // Parse the input LLVM IR file into a module.
    SMDiagnostic Err;
    LLVMContext Context;
    std::unique_ptr <Module> Mod(parseIR(MemoryBufferRef(StringRef(ir), StringRef("id")), Err, Context));
//    std::unique_ptr<Module> Mod(parseIRFile("main.ll", Err, Context));


//    Mod->getFunctionList();
    // Go over all named mdnodes in the module
}

//void Parser::iterate(ilist_node* entity){
//    for (Module::iterator I = entity->begin(),
//                 E = entity->end();
//         I != E; ++I) {
//        outs() << "Found MDNode:\n";
//        I->dump();
//    }
//}