//#include <memory>
//#include "llvm/IR/Metadata.h"
//#include "llvm/IR/Module.h"
//#include "llvm/IRReader/IRReader.h"
//#include "llvm/Pass.h"
//#include "llvm/Support/SourceMgr.h"
//#include "llvm/Support/raw_ostream.h"
//
//using namespace llvm;
//
//int main(int argc, char **argv) {
//
//
//    const char *ir = "define { i64, double } @\"cfunc._ZN8__main__16$3clambda$3e$242Ex\"(i64 %.1) local_unnamed_addr #1 {\nentry:\n  %.14.i = sitofp i64 %.1 to double\n  %.15.i = fadd double %.14.i, 1.000000e+00\n  %.15.fca.0.insert = insertvalue { i64, double } undef, i64 %.1, 0\n  %.15.fca.1.insert = insertvalue { i64, double } %.15.fca.0.insert, double %.15.i, 1\n  ret { i64, double } %.15.fca.1.insert\n}\n\n";
//
//            // Parse the input LLVM IR file into a module.
//    SMDiagnostic Err;
//    LLVMContext Context;
//    std::unique_ptr<Module> Mod(parseIR(MemoryBufferRef(StringRef(ir), StringRef("id")), Err, Context));
////    std::unique_ptr<Module> Mod(parseIRFile("main.ll", Err, Context));
//    if (!Mod) {
//        Err.print(argv[0], errs());
//        return 1;
//    }
//
//
////    Mod->getFunctionList();
//    // Go over all named mdnodes in the module
//
//    return 0;
//}
//
//void iterate(ilist_node* entity){
//    for (Module::iterator I = entity->begin(),
//                 E = entity->end();
//         I != E; ++I) {
//        outs() << "Found MDNode:\n";
//        I->dump();
//    }
//}

#include <stdio.h>


struct tuple {
    int key;
    int value;
    int dummy;
    int dummy2;
};

struct small_tuple {
    long k;
    long v;
    int d;
    tuple t;
};

//int func(int a, int b) {
//    int r = a + b;
//    return r;
//}

small_tuple fu(small_tuple i){
    return i;
}

int main(int argc, char **argv) {

    small_tuple i;
    fu(i);
//    int r = fu(small_tuple());
//    printf("%d", a);
}