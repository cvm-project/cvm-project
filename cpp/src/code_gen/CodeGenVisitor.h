//
// Created by sabir on 21.07.17.
//

#ifndef CPP_CODEGENVISITOR_H
#define CPP_CODEGENVISITOR_H

#include <unordered_map>
#include <regex>
#include "utils/DAGVisitor.h"
#include "dag/DAG.h"
#include "utils/utils.h"
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include <set>


using namespace std;

class CodeGenVisitor : public DAGVisitor {
public:

    void start_visit(DAG *dag) {
        addGenIncludes();
        emitFuncDecl(dag->action);
        tabInd++;
        dag->sink->accept(*this);
        emitFuncEnd(dag->action);

        string final_code;
        final_code +=
                writeHeader() + "\n"
                + writeIncludes() + "\n"
                + "using namespace std;\n"
                + writeTupleDefs() + "\n"
                + writeLLVMFuncDecls() + "\n"
                + writeHelpers() + "\n"
                + body;

        //write out execute.cpp
        ofstream out(GEN_DIR + "execute.cpp");
        out << final_code;
        out.close();

        write_c_execute();

        write_c_executeh();

        writeMakefile();

    }

    void write_c_execute() {
        ofstream out(GEN_DIR + "c_execute.c");
        out << "#include \"c_execute.h\"\n"
                "\n"
                "result_type *execute();\n"
                "\n"
                "void free_result(result_type *ptr);\n"
                "\n"
                "\n"
                "result_type *c_execute() { return execute(); }\n"
                "\n"
                "void c_free_result(result_type *ptr) { return free_result(ptr); }";
        out.close();
    }

    void write_c_executeh(){
        ofstream out(GEN_DIR + "c_execute.h");
        out << resultTypeDef << "\n"
                "result_type *c_execute();\n"
                "\n"
                "void free_result(result_type *);";
        out.close();
    }

    void writeMakefile(){
        ofstream out(GEN_DIR + "Makefile");
        out << "CC = clang-3.7\n"
                "AS = llvm-as-3.7\n"
                "\n"
                "SOURCES = execute.cpp $(wildcard ../src/operators/*.cpp)\n"
                "INC = -I ../src/ -I functions_llvm -I ../src/utils/\n"
                "UDF_SOURCES = $(wildcard functions_llvm/*.ll)\n"
                "\n"
                "OBJECTS = $(SOURCES:.cpp=.bc)\n"
                "UDF_OBJECTS = $(UDF_SOURCES:.ll=.bc)\n"
                "\n"
                "\n"
                "all: execute\n"
                "\n"
                "\n"
                ".PRECIOUS: %.ll\n"
                "\n"
                "%.ll: %.cpp\n"
                "\t$(CC) -S -O3 -emit-llvm $^ -std=c++14  $(INC) -o $@\n"
                "\n"
                "%.bc: %.ll\n"
                "\t$(AS) $^ -o $@\n"
                "\n"
                "c_execute.o: c_execute.c\n"
                "\t$(CC) -c -O3 $^ -o $@ -fPIC\n"
                "\n"
                "\n"
                "execute: $(UDF_OBJECTS) $(OBJECTS) c_execute.o\n"
                "\t$(CC) -flto $^ -o $@.so -lstdc++ -shared\n"
                "\n"
                "#%.asm: %\n"
                "#\tobjdump -D $^ > $@\n"
                "\n"
                "clean:\n"
                "\trm -f *.bc *.ll execute *.asm functions_llvm/*.bc\n"
                "\n"
                ".PHONY: clean";
        out.close();
    }


    void visit(DAGCollection *op) {
        DAGVisitor::visit(op);
        string tType = emitTupleType(op->output_type);
        string opName = getNextOperatorName();
        operatorNameMap.emplace(op->id, make_pair(opName, tType));

        includes.insert("\"operators/CollectionSourceOperator.h\"");

        emitOperatorMake("CollectionSourceOperator", op, opName);
        appendLineBodyNoCol();
    }

    void visit(DAGMap *op) {

        DAGVisitor::visit(op);

        string operatorName = "MapOperator";
        emitComment(operatorName);
        string tType = emitTupleType(op->output_type);
        string opName = getNextOperatorName();
        operatorNameMap.emplace(op->id, make_pair(opName, tType));

        includes.insert("\"operators/MapOperator.h\"");

        storeLLVMCode(op->llvm_ir, "map");
        emitLLVMFunctionWrapper(op, "map");

        emitOperatorMake("MapOperator", op, opName, "map");
        appendLineBodyNoCol();
    }

    void visit(DAGRange *op) {
        DAGVisitor::visit(op);

        string operatorName = "RangeSourceOperator";
        emitComment(operatorName);

        string tType = emitTupleType(op->output_type);
        string opName = getNextOperatorName();
        operatorNameMap.emplace(op->id, make_pair(opName, tType));

        includes.insert("\"operators/RangeSourceOperator.h\"");

        string args = op->from + ", " + op->to + ", " + op->step;
        emitOperatorMake("RangeSourceOperator", op, opName, "", args);
        appendLineBodyNoCol();
    };

    void visit(DAGFilter *op) {
        string operatorName = "FilterOperator";
        emitComment(operatorName);

        DAGVisitor::visit(op);
        string tType = emitTupleType(op->output_type);
        string opName = getNextOperatorName();
        operatorNameMap.emplace(op->id, make_pair(opName, tType));

        includes.insert("\"operators/FilterOperator.h\"");

        storeLLVMCode(op->llvm_ir, "filter");
        emitLLVMFunctionWrapper(op, "filter");

        emitOperatorMake(operatorName, op, opName, "filter");
        appendLineBodyNoCol();
    };


private:


    const string GEN_DIR = "gen/";
    const string LLVM_FUNC_DIR = "functions_llvm/";
    const std::string TUPLE_NAME = "tuple_";
    size_t tupleCounter = -1;

    const std::string OPERATOR_NAME = "op_";
    size_t operatorCounter = -1;

    const std::string LLVM_WRAPPER_NAME = "WrapperFunction_";
    size_t llvmWrapperCounter = -1;

    string LLVMFuncName = "_operator_function_";
    size_t LLVMFuncNameCounter = -1;

    unordered_map<size_t, pair<string, string>> operatorNameMap;

    vector<string> llvmFuncDecls;

    vector<string> tupleTypeDefs;

    set<string> includes;

    size_t tabInd = 0;

    string body;

    string lastTupleType;
    string lastTupleTypeLLVM;
    string resultTypeDef;


    string getCurrentLLVMFuncName() {
        string res;
        res.append(LLVMFuncName)
                .append(to_string(LLVMFuncNameCounter));
        return res;
    }

    string getNextLLVMFuncName() {
        LLVMFuncNameCounter++;
        return getCurrentLLVMFuncName();
    }

    string getCurrentTupleName() {
        string res;
        res.append(TUPLE_NAME)
                .append(to_string(tupleCounter));
        return res;
    }

    string getNextTupleName() {
        tupleCounter++;
        return getCurrentTupleName();
    }

    string getCurrentOperatorName() {
        string res;
        res.append(OPERATOR_NAME)
                .append(to_string(operatorCounter));
        return res;
    }

    string getNextOperatorName() {
        operatorCounter++;
        return getCurrentOperatorName();
    }

    void emitOperatorMake(string opClass, DAGOperator *op, string opVarName, string opName = "",
                          string extraArgs = "") {
        string line("auto ");
        line.append(opVarName)
                .append(" = make")
                .append(opClass)
                .append("<")
                .append(getCurrentTupleName()).append(">(");

        string argList;
        for (auto it = op->predecessors.begin(); it < op->predecessors.end(); it++) {

            argList.append("&" + operatorNameMap[(*it)->id].first);
            if (it != (--op->predecessors.end())) {
                argList.append(", ");
            }
        }

        if (opName.size()) {
            if (argList.size()) {
                argList.append(", ");
            }
            argList.append(opName + snake_to_camel_string(getCurrentLLVMFuncName()))
                    .append("()");
        }

        if (extraArgs.size()) {
            if (argList.size()) {
                argList.append(", ");
            }
            argList.append(extraArgs);
        }

        line.append(argList);
        line.append(")");
        appendLineBody(line);
    }

    string emitTupleType(string type) {
        lastTupleTypeLLVM = type;
        string line("typedef ");
        string tName = getNextTupleName();
        line.append(parseTupleType(type))
                .append(" ")
                .append(tName);
        tupleTypeDefs.push_back(line);
        return tName;
    }

    void emitLLVMFunctionWrapper(DAGOperator *op, string opName) {
        string llvmFuncName = opName + getCurrentLLVMFuncName();
        string className = snake_to_camel_string(llvmFuncName);
        appendLineBodyNoCol("class " + className + " {");
        appendLineBodyNoCol("public:");
        tabInd++;
        string inputType = operatorNameMap[op->predecessors[0]->id].second;
        string retType = operatorNameMap[op->id].second;
        appendLineBodyNoCol(string("auto operator()(")
                                    .append(inputType)
                                    .append(" t) {")
        );
        tabInd++;
        appendLineBody("return call(" + llvmFuncName + ", t)");
        tabInd--;
        appendLineBodyNoCol("}");
        tabInd--;
        appendLineBodyNoCol("};");

        llvmFuncDecls.push_back(retType + " " + llvmFuncName + "(" + inputType + ");");
    }

    string writeHelpers() {
        return "namespace impl {\n"
                "    template<typename Function, typename... Types, size_t... Indexes>\n"
                "    auto call_impl(const Function &f, const std::tuple<Types...> &t,\n"
                "                   const std::integer_sequence<size_t, Indexes...> &) {\n"
                "        return f(std::get<Indexes>(t)...);\n"
                "    }\n"
                "\n"
                "}  // namespace impl\n"
                "\n\n"
                "template<typename Function, typename... Types>\n"
                "auto call(const Function &f, const std::tuple<Types...> &t) {\n"
                "    return impl::call_impl(f, t, std::index_sequence_for<Types...>());\n"
                "}"
                "\n"
                "template<typename Function, typename Type>\n"
                "auto call(const Function &f, const Type &t) {\n"
                "    return f(t);\n"
                "}"
                "\n\n";
    }

    void emitFuncEnd(string action) {
        if (action == "count") {
            emitComment("counting the result");
            appendLineBody("size_t tuple_count = 0");
            appendLineBodyNoCol("while (auto res = " + getCurrentOperatorName() + ".next()) {");
            tabInd++;
            appendLineBody("tuple_count++");
            tabInd--;
            appendLineBodyNoCol("}");
            appendLineBody("return tuple_count");
        } else if (action == "collect") {
            emitComment("copying the result");
            appendLineBody("size_t allocatedSize = 2");
            appendLineBody("size_t resSize = 0");
            appendLineBody(getCurrentTupleName() + " *result = (" + getCurrentTupleName() + " *) malloc(sizeof("
                           + getCurrentTupleName() + ") * allocatedSize))");
            appendLineBodyNoCol("while (auto res = " + getCurrentOperatorName() + ".next()) {");
            tabInd++;
            appendLineBodyNoCol("if (allocatedSize <= resSize) {");
            tabInd++;
            appendLineBody("allocatedSize *= 2");
            appendLineBody("result = (" + getCurrentTupleName() + "*) realloc(result, sizeof(" + getCurrentTupleName() +
                           ") * allocatedSize)");
            tabInd--;
            appendLineBodyNoCol("}");
            appendLineBody("result[resSize] = res.value");
            appendLineBody("resSize++");
            tabInd--;
            appendLineBodyNoCol("}");
            appendLineBody("result_type *ret = (result_type *) malloc(sizeof(result_type))");
            appendLineBody("ret->data = result");
            appendLineBody("ret->size = resSize");
            appendLineBody("return ret");
        }
        tabInd--;
        appendLineBodyNoCol("}");

        appendLineBodyNoCol("void free_result(result_type *ptr) {\n"
                                    "    free(ptr->data);\n"
                                    "    free(ptr);\n"
                                    "    DEBUG_PRINT(\"freeing the result memory\");\n"
                                    "}");
        appendLineBodyNoCol("}");
    }

    void emitComment(string opName) {
        appendLineBodyNoCol("");
        appendLineBodyNoCol("/**" + opName + "**/");
    }

    string parseTupleType(string &type) {
        type = string_replace(type, "(", "tuple<");
        type = string_replace(type, ")", ">");
        return type;
    }

    void storeLLVMCode(string ir, string opName) {
        //the local.. string is not llvm-3.7 compatible:
        regex reg1("local_unnamed_addr #.? ");
        ir = regex_replace(ir, reg1, "");
//        ir = string_replace(ir, "local_unnamed_addr #1 ", "");
        //replace the func name with our
        string funcName = opName.append(getNextLLVMFuncName());
        regex reg("@.*\".*\"");
        ir = regex_replace(ir, reg, "@\"" + funcName + "\"");
        //write code to the gen dir
        if (DUMP_FILES) {
            const int dirErr = system(("mkdir -p " + GEN_DIR + LLVM_FUNC_DIR).c_str());
            if (0 != dirErr) {
                cerr << ("Error creating directory!") << endl;
                exit(1);
            }
            string path = GEN_DIR + LLVM_FUNC_DIR + funcName + ".ll";
            ofstream out(path);
            out << ir;
            out.close();
        }
    }

    void appendLineBody(string str) {
        body.append(string(tabInd, '\t'))
                .append(str)
                .append(";\n");
    }

    void appendLineBodyNoCol(string str = "") {
        body.append(string(tabInd, '\t'))
                .append(str)
                .append("\n");
    }

    void addGenIncludes() {
        includes.insert("<tuple>");
        includes.insert("<vector>");
        includes.insert("\"utils/tuple_to_string.cpp\"");
    }

    string writeHeader() {
        return "/**\n"
                " * Auto-generated execution plan\n"
                " */\n";

    }

    void emitFuncDecl(string action) {
        if (action == "collect") {
            appendLineBodyNoCol("extern \"C\" { " + getCurrentTupleName() + " *execute() {");
        } else if (action == "count") {
            appendLineBodyNoCol("extern \"C\" { size_t execute() {");
        }
    }

    string writeIncludes() {
        string ret;
        for (auto incl : includes) {
            ret.append("#include ")
                    .append(incl)
                    .append("\n");
        }
        ret.append("\n");
        return ret;
    }

    string writeTupleDefs() {
        string ret;
        size_t len = tupleTypeDefs.size();
        for (size_t i = 0; i < len - 1; i++) {
            ret.append(tupleTypeDefs[i])
                    .append(";\n");
        }
        //the last one should be transformed to a struct
        DEBUG_PRINT(lastTupleTypeLLVM);
        lastTupleTypeLLVM = string_replace(lastTupleTypeLLVM, "(", "");
        lastTupleTypeLLVM = string_replace(lastTupleTypeLLVM, ")", "");
        lastTupleTypeLLVM = string_replace(lastTupleTypeLLVM, " ", "");
        vector<string> types = split_string(lastTupleTypeLLVM, ",");
        resultTypeDef = "typedef struct {\n";
        string varName = "v";
        for (size_t i = 0; i < types.size(); i++) {
            resultTypeDef += "\t" + types[i] + " " + varName + to_string(i) + ";\n";
        }
        resultTypeDef += "} " + getCurrentTupleName() + ";\n\n";

        resultTypeDef += "typedef struct {\n "
                                 "\tunsigned int size;\n"
                                 "\t" + getCurrentTupleName() + " *data;\n"
                                 "} result_type;\n";
        return ret + resultTypeDef;
    }

    string writeLLVMFuncDecls() {
        string ret;
        ret.append("extern \"C\" {\n");
        tabInd++;
        for (auto def : llvmFuncDecls) {
            ret.append(def);
        }
        tabInd--;
        ret.append("\n}");
        ret.append("\n");
        return ret;

    }
};


#endif //CPP_CODEGENVISITOR_H
