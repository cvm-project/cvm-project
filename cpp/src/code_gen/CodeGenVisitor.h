//
// Created by sabir on 21.07.17.
//

#ifndef CPP_CODEGENVISITOR_H
#define CPP_CODEGENVISITOR_H

#include <unordered_map>
#include "utils/DAGVisitor.h"

using namespace std;

class CodeGenVisitor : public DAGVisitor {
public:

    void start_visit(DAGOperator *op) {
        emitFuncDecl();
        op->accept(*this);
        emitFuncEnd();
        cout << body << endl;
    }


    void visit(DAGCollection *op) {
        DAGVisitor::visit(op);
        string tType = emitTupleType(op->output_type);
        string opName = getNextOperatorName();
        operatorNameMap.emplace(op->id, make_pair(opName, tType));
        emitOperatorMake("CollectionSourceOperator", op, opName);
        appendLineBodyNoCol();
    }

    void visit(DAGMap *op) {
        DAGVisitor::visit(op);
        string tType = emitTupleType(op->output_type);
        string opName = getNextOperatorName();
        operatorNameMap.emplace(op->id, make_pair(opName, tType));

        storeLLVMCode(op->llvm_ir);
        emitLLVMFunctionWrapper(op);

        emitOperatorMake("MapOperator", op, opName);
        appendLineBodyNoCol();
    }

//    void visit(DAGRange *) {};
//
//    void visit(DAGFilter *) {};

//    void visit(DAGJoin *) {};



private:


    const string genDir = "gen";
    const std::string tupleName = "tuple_";
    size_t tupleCounter = -1;

    const std::string operatorName = "op_";
    size_t operatorCounter = -1;

    const std::string llvmWrapperName = "WrapperFunction_";
    size_t llvmWrapperCounter = -1;

    string LLVMFuncName = "_operator_function_";
    size_t LLVMFuncNameCounter = -1;

    unordered_map<size_t, pair<string, string>> operatorNameMap;

    size_t tabInd = 0;

    string includes;
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
        res.append(tupleName)
                .append(to_string(tupleCounter));
        return res;
    }

    string getNextTupleName() {
        tupleCounter++;
        return getCurrentTupleName();
    }

    string getCurrentOperatorName() {
        string res;
        res.append(operatorName)
                .append(to_string(operatorCounter));
        return res;
    }

    string getNextOperatorName() {
        operatorCounter++;
        return getCurrentOperatorName();
    }

    string getCurrentLLVMFuncName() {
        string res;
        res.append(llvmWrapperName)
                .append(to_string(llvmWrapperCounter));
        return res;
    }

    string getNextLLVMFuncName() {
        llvmWrapperCounter++;
        return getCurrentLLVMFuncName();
    }


    void emitOperatorMake(string opClass, DAGOperator *op, string opName, bool llvmFunc = 0) {
        string line("auto ");
        line.append(opName)
                .append(" = make")
                .append(opClass)
                .append(" <")
                .append(getCurrentTupleName()).append("> (");

        for (auto it = op->predecessors.begin(); it < op->predecessors.end(); it++) {

            line.append(operatorNameMap[(*it)->id].first);
            if (it != (--op->predecessors.end())) {
                line.append(", ");
            }
        }

        if (llvmFunc) {
            line.append(getCurrentLLVMFuncName())
                    .append("()");
        }
        line.append(")");
        appendLineBody(line);
    }

    string emitTupleType(string type) {
        string line("typedef ");
        string tName = getNextTupleName();
        line.append(parseTupleType(type))
                .append(" ")
                .append(tName);
        appendLineBody(line);
        return tName;
    }

    void emitLLVMFunctionWrapper(DAGOperator *op) {
        appendLineBodyNoCol("class " + getNextLLVMFuncName() + " {");
        appendLineBodyNoCol("public:");
        tabInd++;
        string inputType = operatorNameMap[op->predecessors[0]->id].second;
        string llvmFuncName = "MUAHAHA";
        appendLineBodyNoCol(string("auto operator()(")
                                    .append(inputType)
                                    .append(" t) {")
        );
        tabInd++;
        appendLineBody("return call(" + llvmFuncName + ", t)");
        tabInd--;
        appendLineBodyNoCol("}");
        tabInd--;

    }


    void emitFuncDecl() {
        appendLineBodyNoCol("namespace impl {"
                                    "   template<typename Function, typename... Types, size_t... Indexes>"
                                    "   auto call_impl(const Function &f, const std::tuple<Types...> &t,"
                                    "                 const std::integer_sequence<size_t, Indexes...> &) {"
                                    "      return f(std::get<Indexes>(t)...);"
                                    " }"

                                    "}  // namespace impl"

                                    "template<typename Function, typename... Types>"
                                    "auto call(const Function &f, const std::tuple<Types...> &t) {"
                                    "   return impl::call_impl(f, t, std::index_sequence_for<Types...>());"
                                    "})");
        appendLineBodyNoCol();
        appendLineBodyNoCol("void execute() {");
        tabInd++;
    }


    void emitFuncEnd() {
        tabInd--;
        appendLineBodyNoCol("}");
    }

    string parseTupleType(string &type) {
        //replace ( with tuple<
        size_t index = 0;
        while (true) {
            /* Locate the substring to replace. */
            index = type.find("(", index);
            if (index == std::string::npos) break;

            /* Make the replacement. */
            type.replace(index, 1, "tuple<");

            /* Advance index forward so the next iteration doesn't pick it up as well. */
            index += 1;
        }

        // ) with >
        index = 0;
        while (true) {
            /* Locate the substring to replace. */
            index = type.find(")", index);
            if (index == std::string::npos) break;

            type.replace(index, 1, ">");

            index += 1;
        }
        return type;
    }


    void storeLLVMCode(string ir, string opName) {
        //replace the func name with our
        string funcName = opName.append(getNextLLVMFuncName());

        //write code to the gen dir
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

    void appendInclude(string str) {
        includes.append("#include \"")
                .append(str)
                .append("\"\n");
    }

    void appendSysInclude(string str) {
        includes.append("#include <")
                .append(str)
                .append(">\n");
    }
};


#endif //CPP_CODEGENVISITOR_H
