//
// Created by sabir on 21.07.17.
//

#ifndef CPP_CODEGENVISITOR_H
#define CPP_CODEGENVISITOR_H

#include <unordered_map>
#include <regex>
#include "utils/DAGVisitor.h"
#include "utils/utils.h"
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include <set>


using namespace std;

class CodeGenVisitor : public DAGVisitor {
public:

    void start_visit(DAGOperator *op) {
        addGenIncludes();
        emitHelpers();

        emitFuncDecl();
        op->accept(*this);
        emitFuncEnd();

        string final_code;
        final_code +=
                writeHeader() + "\n"
                + writeIncludes() + "\n"
                + "using namespace std;\n"
                + writeTupleDefs() + "\n"
                + writeLLVMFuncDecls() + "\n"
                + body;

        cout << final_code;

        ofstream out(GEN_DIR + "execute.cpp");
        out << final_code;
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
        emitOperatorComment(operatorName);
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
        emitOperatorComment(operatorName);

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
        emitOperatorComment(operatorName);

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

//    void visit(DAGJoin *) {};



private:


    const string GEN_DIR = "cpp/gen/";
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

    void emitHelpers() {
        appendLineBodyNoCol("namespace impl {\n"
                                    "    template<typename Function, typename... Types, size_t... Indexes>\n"
                                    "    auto call_impl(const Function &f, const std::tuple<Types...> &t,\n"
                                    "                   const std::integer_sequence<size_t, Indexes...> &) {\n"
                                    "        return f(std::get<Indexes>(t)...);\n"
                                    "    }\n"
                                    "\n"
                                    "}  // namespace impl\n"
                                    "\n"
                                    "template<typename Function, typename... Types>\n"
                                    "auto call(const Function &f, const std::tuple<Types...> &t) {\n"
                                    "    return impl::call_impl(f, t, std::index_sequence_for<Types...>());\n"
                                    "}"
                                    "\n"
                                    "template<typename Function, typename Type>\n"
                                    "auto call(const Function &f, const Type &t) {\n"
                                    "    return f(t);\n"
                                    "}"
                                    "\n");
        appendLineBodyNoCol();
    }

    void emitFuncDecl() {
        appendLineBodyNoCol("extern \"C\" void execute() {");
        tabInd++;
    }

    void emitFuncEnd() {

        appendLineBody(getCurrentOperatorName() + ".open()");
        appendLineBodyNoCol("while (auto res = " + getCurrentOperatorName() + ".next()) {");
        tabInd++;
        appendLineBody("cout << to_string(res.value) << endl");
        tabInd--;
        appendLineBodyNoCol("}");
        tabInd--;
        appendLineBodyNoCol("}");
    }

    void emitOperatorComment(string opName) {
        appendLineBodyNoCol("\n/**" + opName + "**/");
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
        for (auto def : tupleTypeDefs) {
            ret.append(def)
                    .append(";\n");
        }
        return ret;
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
