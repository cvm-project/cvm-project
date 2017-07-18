//
// Created by sabir on 18.07.17.
//

#ifndef CPP_PARSER_H
#define CPP_PARSER_H

#include <memory>
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class Parser {
public:
    void parse();

private:
//    void iterate(ilist_node*);
};


#endif //CPP_PARSER_H
