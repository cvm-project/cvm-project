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

    CodeGenVisitor() : genDir(get_lib_path() + "cpp/gen/") {}

    void start_visit(DAG *dag) {

        makeDirectory();
        addGenIncludes();
        tabInd++;
        dag->sink->accept(*this);
        emitFuncEnd(dag->action);

        string final_code;
        final_code +=
                writeHeader() + "\n"
                + writeIncludes() + "\n"
                + "using namespace std;\n"
                + writeTupleDefs(dag->action) + "\n"
                + writeLLVMFuncDecls() + "\n"
                //                + writeHelpers() + "\n"
                + writeFuncDecl() + "\n"
                + body;

        write_execute(final_code);

        write_c_execute(dag->action);

        write_c_executeh(dag->action);

        writeMakefile();

    }

    void visit(DAGCollection *op) {

        string operatorName = "CollectionSourceOperator";
        DAGVisitor::visit(op);
        emitComment(operatorName);

        string opName = getNextOperatorName();
        auto tType = generateTupleType(op->output_type);
        operatorNameTupleTypeMap.emplace(op->id, make_tuple(opName, tType.first, tType.second));

        includes.insert("\"operators/" + operatorName + ".h\"");
        pair<string, string> inputNamePair = getNextInputName();
        emitOperatorMake(operatorName, op, opName, "",
                         "(" + tType.first + "*)" + inputNamePair.first + ", " + inputNamePair.second);
        inputNames.push_back(inputNamePair);
        appendLineBodyNoCol();
    }

    void visit(DAGMap *op) {

        string operatorName = "MapOperator";
        DAGVisitor::visit(op);
        emitComment(operatorName);

        string opName = getNextOperatorName();

        auto tType = generateTupleType(op->output_type);
        operatorNameTupleTypeMap.emplace(op->id, make_tuple(opName, tType.first, tType.second));

        includes.insert("\"operators/" + operatorName + ".h\"");

        storeLLVMCode(op->llvm_ir, "map");
        emitLLVMFunctionWrapper(op, "map");

        emitOperatorMake(operatorName, op, opName, "map");
        appendLineBodyNoCol();
    }

    void visit(DAGReduce *op) {

        string operatorName = "ReduceOperator";

        DAGVisitor::visit(op);
        emitComment(operatorName);

        string opName = getNextOperatorName();

        auto tType = get<1>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
        auto typeSize = get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
        operatorNameTupleTypeMap.emplace(op->id, make_tuple(opName, tType, typeSize));

        includes.insert("\"operators/" + operatorName + ".h\"");

        storeLLVMCode(op->llvm_ir, "reduce");
        emitLLVMFunctionWrapperBinaryArgs(op, "reduce");

        emitOperatorMake(operatorName, op, opName, "reduce");
        appendLineBodyNoCol();
    }

    void visit(DAGRange *op) {
        string operatorName = "FilterOperator";

        DAGVisitor::visit(op);
        emitComment(operatorName);

        string opName = getNextOperatorName();
        auto tType = generateTupleType(op->output_type);
        operatorNameTupleTypeMap.emplace(op->id, make_tuple(opName, tType.first, tType.second));

        includes.insert("\"operators/" + operatorName + ".h\"");

        string args = op->from + ", " + op->to + ", " + op->step;
        emitOperatorMake(operatorName, op, opName, "", args);
        appendLineBodyNoCol();
    };

    void visit(DAGFilter *op) {
        string operatorName = "FilterOperator";

        DAGVisitor::visit(op);
        emitComment(operatorName);

        string opName = getNextOperatorName();
        auto tType = operatorNameTupleTypeMap[op->predecessors[0]->id];
        operatorNameTupleTypeMap.emplace(op->id, make_tuple(opName, get<1>(tType), get<2>(tType)));

        includes.insert("\"operators/" + operatorName + ".h\"");

        storeLLVMCode(op->llvm_ir, "filter");
        emitLLVMFunctionWrapper(op, "filter", true);

        emitOperatorMake(operatorName, op, opName, "filter");
        appendLineBodyNoCol();
    };


private:


    const string genDir;
    const string LLVM_FUNC_DIR = "functions_llvm/";
    const std::string TUPLE_NAME = "tuple_";
    size_t tupleCounter = -1;

    const std::string OPERATOR_NAME = "op_";
    size_t operatorCounter = -1;

    const std::string LLVM_WRAPPER_NAME = "WrapperFunction_";
    size_t llvmWrapperCounter = -1;

    string LLVMFuncName = "_operator_function_";
    size_t LLVMFuncNameCounter = -1;

    unordered_map<size_t, tuple<string, string, size_t>> operatorNameTupleTypeMap;

    vector<string> llvmFuncDecls;

    vector<string> tupleTypeDefs;

    vector<pair<string, string>> inputNames;

    set<string> includes;

    size_t tabInd = 0;

    string body;

    string lastTupleType;
    string lastTupleTypeLLVM;

    string resultTypeDef;

    string executeFuncReturn = "result_type";
    string executeFuncParams;

    const string INPUT_NAME = "input_";
    const string INPUT_NAME_SIZE = "input_size_";
    size_t inputNameCounter = -1;


    auto getCurrentInputNamePair() {
        return make_pair(INPUT_NAME + to_string(inputNameCounter), INPUT_NAME_SIZE + to_string(inputNameCounter));
    }

    pair<string, string> getNextInputName() {
        inputNameCounter++;
        return getCurrentInputNamePair();
    }

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

            argList.append("&" + get<0>(operatorNameTupleTypeMap[(*it)->id]));
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

    /***
     * generate a struct type
     * returns the tuple type name(e.g tuple_0) and the number of fields in this tuple
     */
    pair<string, size_t> generateTupleType(const string &type) {
        string tName = getNextTupleName();
        string type_ = type;
        type_ = string_replace(type_, "(", "");
        type_ = string_replace(type_, ")", "");
        type_ = string_replace(type_, " ", "");
        vector<string> types = split_string(type_, ",");
        string line("typedef struct {\n");
        string varName = "v";
        for (size_t i = 0; i < types.size(); i++) {
            line += "\t" + types[i] + " " + varName + to_string(i) + ";\n";
        }
        line += "} " + tName + ";\n\n";
        tupleTypeDefs.push_back(line);
        return make_pair(tName, types.size());
    }

    void emitLLVMFunctionWrapper(DAGOperator *op, string opName, bool returnsBool = false) {
        string llvmFuncName = opName + getCurrentLLVMFuncName();
        string className = snake_to_camel_string(llvmFuncName);
        appendLineBodyNoCol("class " + className + " {");
        appendLineBodyNoCol("public:");
        tabInd++;
        string inputType = get<1>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
        string retType = get<1>(operatorNameTupleTypeMap[op->id]);
        appendLineBodyNoCol(string("auto operator()(")
                                    .append(inputType)
                                    .append(" t) {")
        );
        tabInd++;
        string args = "";
        string varName = "v";
        for (size_t i = 0; i < get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]); i++) {
            args += "t." + varName + to_string(i) + ", ";
        }
        if (args != "") {
            args.pop_back();
            args.pop_back();
        }
        appendLineBody("return " + llvmFuncName + "(" + args + ")");
        tabInd--;
        appendLineBodyNoCol("}");
        tabInd--;
        appendLineBodyNoCol("};");

        string flatInputType = string_replace(op->predecessors[0]->output_type, "(", "");
        flatInputType = string_replace(flatInputType, ")", "");
        if (returnsBool) {
            llvmFuncDecls.push_back("bool " + llvmFuncName + "(" + flatInputType + ");");
        } else {
            llvmFuncDecls.push_back(retType + " " + llvmFuncName + "(" + flatInputType + ");");
        }
    }

    /**
     * functions which take two arguments of the same type (e.g. reduce)
     */
    void emitLLVMFunctionWrapperBinaryArgs(DAGOperator *op, string opName) {
        string llvmFuncName = opName + getCurrentLLVMFuncName();
        string className = snake_to_camel_string(llvmFuncName);
        appendLineBodyNoCol("class " + className + " {");
        appendLineBodyNoCol("public:");
        tabInd++;
        string inputType = get<1>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
        string retType = get<1>(operatorNameTupleTypeMap[op->id]);
        appendLineBodyNoCol(string("auto operator()(")
                                    .append(inputType + " t0, ")
                                    .append(inputType + " t1) {")
        );
        tabInd++;
        string args = "";
        string varName = "v";
        for (size_t i = 0; i < get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]); i++) {
            args += "t0." + varName + to_string(i) + ", ";
        }

        for (size_t i = 0; i < get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]); i++) {
            args += "t1." + varName + to_string(i) + ", ";
        }

        if (args != "") {
            args.pop_back();
            args.pop_back();
        }
        appendLineBody("return " + llvmFuncName + "(" + args + ")");
        tabInd--;
        appendLineBodyNoCol("}");
        tabInd--;
        appendLineBodyNoCol("};");

        string flatInputType = string_replace(op->predecessors[0]->output_type, "(", "");
        flatInputType = string_replace(flatInputType, ")", "");
        llvmFuncDecls.push_back(retType + " " + llvmFuncName + "(" + flatInputType + ", " + flatInputType + ");");
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
                "}\n"
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
            appendLineBody(getCurrentOperatorName() + ".open()");
            appendLineBody("result_type tuple_count = 0");
            appendLineBodyNoCol("while (auto res = " + getCurrentOperatorName() + ".next()) {");
            tabInd++;
            appendLineBody("tuple_count++");
            tabInd--;
            appendLineBodyNoCol("}");
            appendLineBody(getCurrentOperatorName() + ".close()");
            appendLineBody("return tuple_count");
            tabInd--;
            appendLineBodyNoCol("}");
        } else if (action == "reduce") {
            emitComment("reduce");
            appendLineBody(getCurrentOperatorName() + ".open()");
            appendLineBody("auto res = " + getCurrentOperatorName() + ".next()");
            appendLineBody(getCurrentOperatorName() + ".close()");
            appendLineBody("result_type ret = (result_type) malloc(sizeof(" + getCurrentTupleName() + "))");
            appendLineBody("*ret = res");
            appendLineBody("return ret");
            tabInd--;
            appendLineBodyNoCol("}");
        } else if (action == "collect") {
            emitComment("copying the result");
            appendLineBody(getCurrentOperatorName() + ".open()");
            appendLineBody("size_t allocatedSize = 2");
            appendLineBody("size_t resSize = 0");
            appendLineBody(getCurrentTupleName() + " *result = (" + getCurrentTupleName() + " *) malloc(sizeof("
                           + getCurrentTupleName() + ") * allocatedSize)");
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
            appendLineBody(getCurrentOperatorName() + ".close()");
            appendLineBody("result_type ret = (result_type) malloc(sizeof(result_struct))");
            appendLineBody("ret->data = result");
            appendLineBody("ret->size = resSize");
            appendLineBody("return ret");

            tabInd--;
            appendLineBodyNoCol("}");

            appendLineBodyNoCol("void free_result(result_type ptr) {\n"
                                        "    DEBUG_PRINT(\"freeing the result memory\");\n"
                                        "    if (ptr != NULL && ptr->data != NULL) {\n"
                                        "        free(ptr->data);\n"
                                        "        ptr->data = NULL;\n"
                                        "    }\n"
                                        "    if (ptr != NULL) {\n"
                                        "        free(ptr);\n"
                                        "    }\n"
                                        "    ptr = NULL;\n"
                                        "}");
        }
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

    void makeDirectory() {

        if (DUMP_FILES) {
            const int dirErr = system(("mkdir -p " + genDir + LLVM_FUNC_DIR).c_str());
            if (0 != dirErr) {
                cerr << ("Error creating gen directory!") << endl;
                exit(1);
            }
        }
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
            string path = genDir + LLVM_FUNC_DIR + funcName + ".ll";
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

    string writeFuncDecl() {
        executeFuncParams = "";
        for (auto inputPair : inputNames) {
            executeFuncParams += "void *" + inputPair.first + ", unsigned long " + inputPair.second + ", ";
        }
        if (executeFuncParams != "") {
            executeFuncParams.pop_back();
            executeFuncParams.pop_back();
        }
        return "extern \"C\" { " + executeFuncReturn + " execute(" + executeFuncParams + ") {";
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

    string writeTupleDefs(string action) {
        string ret;
        if (action == "collect") {

            size_t len = tupleTypeDefs.size();
            for (size_t i = 0; i < len - 1; i++) {
                ret.append(tupleTypeDefs[i]);
            }
            resultTypeDef = tupleTypeDefs[len - 1];
            resultTypeDef += "\ntypedef struct {\n "
                                     "\tunsigned long size;\n"
                                     "\t" + getCurrentTupleName() + " *data;\n"
                                     "} result_struct;\n";
            resultTypeDef += "typedef result_struct* result_type;\n";
        } else if (action == "count") {
            size_t len = tupleTypeDefs.size();
            for (size_t i = 0; i < len; i++) {
                ret.append(tupleTypeDefs[i])
                        .append(";\n");
            }
            resultTypeDef = "typedef unsigned long result_type;\n";
        } else if (action == "reduce") {
            size_t len = tupleTypeDefs.size();
            for (size_t i = 0; i < len - 1; i++) {
                ret.append(tupleTypeDefs[i])
                        .append(";\n");
            }
            resultTypeDef = tupleTypeDefs[len - 1];
            resultTypeDef += "typedef " + getCurrentTupleName() + "* result_type;\n";
        }
        return ret + resultTypeDef;
    }

    string writeLLVMFuncDecls() {
        string ret;
        ret.append("extern \"C\" {\n");
        tabInd++;
        for (auto def : llvmFuncDecls) {
            ret.append(def + "\n");
        }
        tabInd--;
        ret.append("}\n");
        return ret;

    }


    void write_execute(string final_code) {
        ofstream out(genDir + "execute.cpp");
        out << final_code;
        out.close();
    }

    void write_c_execute(string action) {
        ofstream out(genDir + "c_execute.c");
        string funcParamNames = "";
        for (auto param : inputNames) {
            funcParamNames += param.first + ", " + param.second + ", ";
        }
        if (funcParamNames != "") {
            funcParamNames.pop_back();
            funcParamNames.pop_back();
        }
        out << "#include \"c_execute.h\"\n"
                       "\n"
                       "" + executeFuncReturn + " execute(" + executeFuncParams + ");\n"
                       "\n"
                       "" + executeFuncReturn + " c_execute(" + executeFuncParams + ") { return execute(" +
               funcParamNames + "); }\n";
        if (action == "collect") {
            out << "void free_result(" + executeFuncReturn + " ptr);\n"
                    "\n"
                    "void c_free_result(" + executeFuncReturn + " ptr) { free_result(ptr); }";
        }
        out.close();
    }

    void write_c_executeh(string action) {
        ofstream out(genDir + "c_execute.h");
        out << resultTypeDef << "\n"
                                        "" + executeFuncReturn + " c_execute(" + executeFuncParams + ");\n";
        if (action == "collect") {
            out << "\n"
                           "void c_free_result(" + executeFuncReturn + ");";
        }
        out.close();
    }

    void writeMakefile() {
        ofstream out(genDir + "Makefile");
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


};


#endif //CPP_CODEGENVISITOR_H
