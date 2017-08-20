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
                //                + "using namespace std;\n"
                + writeTupleDefs(dag->action) + "\n"
                + writeLLVMFuncDecls() + "\n"
                + writeFuncDecl() + "\n"
                + body;

        write_execute(final_code);

        write_c_execute(dag->action);

        write_c_executeh(dag->action);

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

        DAGVisitor::visit(op);
        string operatorName = "MapOperator";
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
        emitLLVMFunctionWrapperBinaryArgs(op, "reduce", get<1>(operatorNameTupleTypeMap[op->predecessors[0]->id]),
                                          get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]));

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


    void visit(DAGJoin *op) {
        DAGVisitor::visit(op);
        string operatorName = "JoinOperator";
        emitComment(operatorName);

        string opName = getNextOperatorName();
        auto tType = generateTupleType(op->output_type);
        operatorNameTupleTypeMap.emplace(op->id, make_tuple(opName, tType.first, tType.second));

        //add join k1, v1, v2 tuple types
        join_type_counter++;

        generateJoinKeyValueTypes(op);

        includes.insert("\"operators/" + operatorName + ".h\"");

        emitJoinMake(opName, op);
        appendLineBodyNoCol();
    };


    void visit(DAGReduceByKey *op) {
        DEBUG_PRINT("reduce by key called");
        DAGVisitor::visit(op);
        string operatorName = "ReduceByKeyOperator";
        emitComment(operatorName);

        string opName = getNextOperatorName();
        auto tType = generateTupleType(op->output_type);
        operatorNameTupleTypeMap.emplace(op->id, make_tuple(opName, tType.first, tType.second));

        //add reducebykey key and value tuple types
        reduce_by_key_type_counter++;

        generateReduceByKeyKeyValueTypes(op);

        storeLLVMCode(op->llvm_ir, "reduce_by_key");
        //assume the key is a scalar type
        emitLLVMFunctionWrapperBinaryArgs(op, "reduce_by_key", getCurrentReduceByKeyValueName(),
                                          get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]) - 1, true);

        includes.insert("\"operators/" + operatorName + ".h\"");

        emitReduceByKeyMake(opName, op);
        appendLineBodyNoCol();
    };

private:


    const string genDir;
    const string LLVM_FUNC_DIR = "functions_llvm/";

    string body;

    const std::string TUPLE_NAME = "tuple_";
    size_t tupleCounter = -1;

    const std::string OPERATOR_NAME = "op_";
    size_t operatorCounter = -1;

    const std::string LLVM_WRAPPER_NAME = "WrapperFunction_";
    size_t llvmWrapperCounter = -1;

    string LLVMFuncName = "_operator_function_";
    size_t LLVMFuncNameCounter = -1;

    string JOIN_KEY_NAME = "join_key_";
    string JOIN_VALUE1_NAME = "join_value_0_";
    string JOIN_VALUE2_NAME = "join_value_1_";
    size_t join_type_counter = -1;


    string REDUCE_BY_KEY_KEY_NAME = "reduce_by_key_key_";
    string REDUCE_BY_KEY_VALUE_NAME = "reduce_by_key_value_";
    size_t reduce_by_key_type_counter = -1;


    unordered_map<size_t, tuple<string, string, size_t>> operatorNameTupleTypeMap;

    vector<string> llvmFuncDecls;

    vector<string> tupleTypeDefs;

//    vector<string> joinKeyValuesTypeDefs;

    vector<string> reduceByKeyKeyValuesTypeDefs;

    vector<pair<string, string>> inputNames;

    set<string> includes;

    size_t tabInd = 0;

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

    string getCurrentJoinKeyName() {
        string res;
        res.append(JOIN_KEY_NAME)
                .append(to_string(join_type_counter));
        return res;
    }

    string getCurrentJoinValue1Name() {
        string res;
        res.append(JOIN_VALUE1_NAME)
                .append(to_string(join_type_counter));
        return res;
    }

    string getCurrentJoinValue2Name() {
        string res;
        res.append(JOIN_VALUE2_NAME)
                .append(to_string(join_type_counter));
        return res;
    }

    string getCurrentReduceByKeyKeyName() {
        string res;
        res.append(REDUCE_BY_KEY_KEY_NAME)
                .append(to_string(reduce_by_key_type_counter));
        return res;
    }

    string getCurrentReduceByKeyValueName() {
        string res;
        res.append(REDUCE_BY_KEY_VALUE_NAME)
                .append(to_string(reduce_by_key_type_counter));
        return res;
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

    void emitJoinMake(string opVarName, DAGJoin *op) {
        string line("auto ");
        line.append(opVarName)
                .append(" = make")
                .append("JoinOperator")
                .append("<")
                .append(getCurrentTupleName() + ", " +
                        getCurrentJoinKeyName() + ", " +
                        getCurrentJoinValue1Name() + ", " +
                        getCurrentJoinValue2Name())
                .append(">(");

        string argList;
        for (auto it = op->predecessors.begin(); it < op->predecessors.end(); it++) {

            argList.append("&" + get<0>(operatorNameTupleTypeMap[(*it)->id]));
            if (it != (--op->predecessors.end())) {
                argList.append(", ");
            }
        }

        line.append(argList);
        line.append(")");
        appendLineBody(line);
    }

    void emitReduceByKeyMake(string opVarName, DAGReduceByKey *op) {
        string line("auto ");
        line.append(opVarName)
                .append(" = make")
                .append("ReduceByKeyOperator")
                .append("<")
                .append(getCurrentTupleName() + ", " +
                        getCurrentReduceByKeyKeyName() + ", " +
                        getCurrentReduceByKeyValueName())
                .append(">(");

        string argList;
        for (auto it = op->predecessors.begin(); it < op->predecessors.end(); it++) {

            argList.append("&" + get<0>(operatorNameTupleTypeMap[(*it)->id]));
            argList.append(", ");
        }

        argList.append("reduceByKey" + snake_to_camel_string(getCurrentLLVMFuncName()))
                .append("()");

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
        resultTypeDef = line;
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
        }
        else {
            llvmFuncDecls.push_back(retType + " " + llvmFuncName + "(" + flatInputType + ");");
        }
    }

    /**
     * functions which take two arguments of the same type (e.g. reduce)
     */
    void emitLLVMFunctionWrapperBinaryArgs(DAGOperator *op, string opName, string inputType, size_t inputSize,
                                           bool reduce_by_key = false) {
        string llvmFuncName = opName + getCurrentLLVMFuncName();
        string className = snake_to_camel_string(llvmFuncName);
        appendLineBodyNoCol("class " + className + " {");
        appendLineBodyNoCol("public:");
        tabInd++;
        string retType = get<1>(operatorNameTupleTypeMap[op->id]);
        appendLineBodyNoCol(string("auto operator()(")
                                    .append(inputType + " t0, ")
                                    .append(inputType + " t1) {")
        );
        tabInd++;
        string args = "";
        string varName = "v";
        for (size_t i = 0; i < inputSize; i++) {
            args += "t0." + varName + to_string(i) + ", ";
        }

        for (size_t i = 0; i < inputSize; i++) {
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
        if (reduce_by_key) {
            string flatInputType = string_replace(op->predecessors[0]->output_type, "(", "");
            flatInputType = string_replace(flatInputType, ")", "");
            regex reg(".*,");
            flatInputType = regex_replace(flatInputType, reg, "");
            llvmFuncDecls.push_back(inputType + " " + llvmFuncName + "(" + flatInputType + ", " + flatInputType + ");");
        }
        else {
            string flatInputType = string_replace(op->predecessors[0]->output_type, "(", "");
            flatInputType = string_replace(flatInputType, ")", "");
            llvmFuncDecls.push_back(retType + " " + llvmFuncName + "(" + flatInputType + ", " + flatInputType + ");");
        }
    }

    void emitFuncEnd(string action) {
//        appendLineBodyNoCol("TICK1");
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

//            appendLineBodyNoCol("TOCK1");

            appendLineBody("return tuple_count");
            tabInd--;
            appendLineBodyNoCol("}");
        }
        else if (action == "reduce") {
            emitComment("reduce");
            appendLineBody(getCurrentOperatorName() + ".open()");
            appendLineBody("auto res = " + getCurrentOperatorName() + ".next()");
            appendLineBody(getCurrentOperatorName() + ".close()");
//            appendLineBodyNoCol("TOCK1");
//            appendLineBody("std::cout << \"outer loop time: \"<<DIFF1 << std::endl");
            appendLineBody("result_type ret = (result_type) malloc(sizeof(" + getCurrentTupleName() + "))");
            appendLineBody("*ret = res");
            appendLineBody("return ret");
            tabInd--;
            appendLineBodyNoCol("}");
        }
        else if (action == "collect") {
            emitComment("collecting the result");
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

    /**
     * generate Key, Value1, Value2 struct types
     * for current join
     */
    void generateJoinKeyValueTypes(DAGJoin *op) {
        string up1Type = op->predecessors[0]->output_type;
        string up2Type = op->predecessors[1]->output_type;

        //key1 type
        string keyName = getCurrentJoinKeyName();
        vector<string> keyTypes;
        if (up1Type.find("((") == 0) {
            //key is a tuple
            regex reg("((.*)");
            std::smatch m;
            regex_search(up1Type, m, reg);
            string typesStr;
            for (auto x:m) typesStr = x;
            typesStr = string_replace(typesStr, "(", " ");
            typesStr = string_replace(typesStr, ")", " ");
            typesStr = string_replace(typesStr, " ", "");
            keyTypes = split_string(typesStr, ",");
        }
        else {
            //key is the first element
            string typesStr = string_replace(up1Type, "(", " ");
            typesStr = string_replace(typesStr, ")", " ");
            typesStr = string_replace(typesStr, " ", "");
            keyTypes.push_back(split_string(typesStr, ",")[0]);
        }
        string line("typedef struct {\n");
        string varName = "v";
        for (size_t i = 0; i < keyTypes.size(); i++) {
            line += "\t" + keyTypes[i] + " " + varName + to_string(i) + ";\n";
        }
        line += "} " + keyName + ";\n\n";

        //up1 type
        vector<string> up1Types;
        if (up1Type.find("((") == 0) {
            //assume for now the key is a scalar type
        }
        else {
            string typesStr = string_replace(up1Type, "(", " ");
            typesStr = string_replace(typesStr, ")", " ");
            typesStr = string_replace(typesStr, " ", "");
            up1Types = split_string(typesStr, ",");
            up1Types = vector<string>(up1Types.begin() + 1, up1Types.end());
            line += "typedef struct {\n";

            for (size_t i = 0; i < up1Types.size(); i++) {
                line += "\t" + up1Types[i] + " " + varName + to_string(i) + ";\n";
            }
            line += "} " + getCurrentJoinValue1Name() + ";\n\n";
        }

        //up2 type
        vector<string> up2Types;
        if (up2Type.find("((") == 0) {
            //assume for now the key is a scalar type
        }
        else {
            string typesStr = string_replace(up2Type, "(", " ");
            typesStr = string_replace(typesStr, ")", " ");
            typesStr = string_replace(typesStr, " ", "");
            up2Types = split_string(typesStr, ",");
            up2Types = vector<string>(up2Types.begin() + 1, up2Types.end());
            line += "typedef struct {\n";

            for (size_t i = 0; i < up2Types.size(); i++) {
                line += "\t" + up2Types[i] + " " + varName + to_string(i) + ";\n";
            }
            line += "} " + getCurrentJoinValue2Name() + ";\n\n";
        }

        tupleTypeDefs.push_back(line);
    }

    /**
     * generate Key and Value struct types
     * for current reduce by key
     */
    void generateReduceByKeyKeyValueTypes(DAGReduceByKey *op) {
        string upType = op->predecessors[0]->output_type;

        //key type
        string keyName = getCurrentReduceByKeyKeyName();
        vector<string> keyTypes;
        if (upType.find("((") == 0) {
            //key is a tuple
            regex reg("((.*)");
            std::smatch m;
            regex_search(upType, m, reg);
            string typesStr;
            for (auto x:m) typesStr = x;
            typesStr = string_replace(typesStr, "(", " ");
            typesStr = string_replace(typesStr, ")", " ");
            typesStr = string_replace(typesStr, " ", "");
            keyTypes = split_string(typesStr, ",");
        }
        else {
            //key is the first element
            string typesStr = string_replace(upType, "(", " ");
            typesStr = string_replace(typesStr, ")", " ");
            typesStr = string_replace(typesStr, " ", "");
            keyTypes.push_back(split_string(typesStr, ",")[0]);
        }
        string line("typedef struct {\n");
        string varName = "v";
        for (size_t i = 0; i < keyTypes.size(); i++) {
            line += "\t" + keyTypes[i] + " " + varName + to_string(i) + ";\n";
        }
        line += "} " + keyName + ";\n\n";

        //upstream type
        vector<string> upTypes;
        if (upType.find("((") == 0) {
            //assume for now the key is a scalar type
        }
        else {
            string typesStr = string_replace(upType, "(", " ");
            typesStr = string_replace(typesStr, ")", " ");
            typesStr = string_replace(typesStr, " ", "");
            upTypes = split_string(typesStr, ",");
            upTypes = vector<string>(upTypes.begin() + 1, upTypes.end());
            line += "typedef struct {\n";

            for (size_t i = 0; i < upTypes.size(); i++) {
                line += "\t" + upTypes[i] + " " + varName + to_string(i) + ";\n";
            }
            line += "} " + getCurrentReduceByKeyValueName() + ";\n\n";
        }

        tupleTypeDefs.push_back(line);
    }

    string genComment(string com) {
        return "\n/**" + com + "**/\n";
    }

    void emitComment(string opName) {
        appendLineBodyNoCol(genComment(opName));
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
//        includes.insert("<vector>");
    }

    string writeHeader() {
        return "/**\n"
                " * Auto-generated execution plan\n"
                " */\n";

    }

//    string writeJoinTupleDefs() {
//        string line = "";
//        line += genComment("join key value definitions");
//        for (auto tupleDef : joinKeyValuesTypeDefs) {
//            line += tupleDef;
//        }
//        return line;
//    }

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
        string resultWrapper;
        ret.append(genComment("tuple definitions"));
        if (action == "collect") {

            size_t len = tupleTypeDefs.size();
            for (size_t i = 0; i < len; i++) {
                ret.append(tupleTypeDefs[i]);
            }
            resultWrapper = "\ntypedef struct {\n "
                                    "\tunsigned long size;\n"
                                    "\t" + getCurrentTupleName() + " *data;\n"
                                    "} result_struct;\n"
                                    "typedef result_struct* result_type;\n";
        }
        else if (action == "count") {
            size_t len = tupleTypeDefs.size();
            for (size_t i = 0; i < len; i++) {
                ret.append(tupleTypeDefs[i])
                        .append(";\n");
            }
            resultWrapper = "typedef unsigned long result_type;\n";
        }
        else if (action == "reduce") {
            size_t len = tupleTypeDefs.size();
            for (size_t i = 0; i < len; i++) {
                ret.append(tupleTypeDefs[i])
                        .append(";\n");
            };
            resultWrapper = "typedef " + getCurrentTupleName() + "* result_type;\n";
        }
        resultTypeDef += resultWrapper;
        return ret + resultWrapper;
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


};


#endif //CPP_CODEGENVISITOR_H
