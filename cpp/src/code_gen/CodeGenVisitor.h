//
// Created by sabir on 21.07.17.
//

#ifndef CODE_GEN_CODEGENVISITOR_H
#define CODE_GEN_CODEGENVISITOR_H

#include <cstdlib>
#include <fstream>
#include <regex>
#include <set>
#include <unordered_map>
#include <utility>

#include <sys/stat.h>

#include "config.h"
#include "dag/DAG.h"
#include "utils/DAGVisitor.h"
#include "utils/utils.h"

class CodeGenVisitor : public DAGVisitor {
public:
    CodeGenVisitor() : genDir(get_lib_path() + "/cpp/gen/") {}

    void start_visit(DAG *dag);
    void visit(DAGCollection *op) override;
    void visit(DAGMap *op) override;
    void visit(DAGReduce *op) override;
    void visit(DAGRange *op) override;
    void visit(DAGFilter *op) override;
    void visit(DAGJoin *op) override;
    void visit(DAGCartesian *op) override;
    void visit(DAGReduceByKey *op) override;

private:
    const std::string genDir;
    const std::string LLVM_FUNC_DIR = "functions_llvm/";

    std::string body;

    const std::string TUPLE_NAME = "tuple_";
    size_t tupleCounter = -1;

    const std::string OPERATOR_NAME = "op_";
    size_t operatorCounter = -1;

    const std::string LLVM_WRAPPER_NAME = "WrapperFunction_";
    size_t llvmWrapperCounter = -1;

    std::string LLVMFuncName = "_operator_function_";
    size_t LLVMFuncNameCounter = -1;

    std::string JOIN_KEY_NAME = "join_key_";
    std::string JOIN_VALUE1_NAME = "join_value_0_";
    std::string JOIN_VALUE2_NAME = "join_value_1_";
    size_t join_type_counter = -1;

    std::string REDUCE_BY_KEY_KEY_NAME = "reduce_by_key_key_";
    std::string REDUCE_BY_KEY_VALUE_NAME = "reduce_by_key_value_";
    size_t reduce_by_key_type_counter = -1;

    std::unordered_map<size_t, std::tuple<std::string, std::string, size_t>>
            operatorNameTupleTypeMap;

    std::vector<std::string> llvmFuncDecls;

    std::vector<std::string> tupleTypeDefs;

    //    std::vector<std::string> joinKeyValuesTypeDefs;

    std::vector<std::string> reduceByKeyKeyValuesTypeDefs;

    std::vector<std::pair<std::string, std::string>> inputNames;

    std::set<std::string> includes;

    size_t tabInd = 0;

    std::string lastTupleType;
    std::string lastTupleTypeLLVM;

    std::string resultTypeDef;

    std::string executeFuncReturn = "result_type";
    std::string executeFuncParams;

    const std::string INPUT_NAME = "input_";
    const std::string INPUT_NAME_SIZE = "input_size_";
    size_t inputNameCounter = -1;

    auto getCurrentInputNamePair() {
        return std::make_pair(
                INPUT_NAME + std::to_string(inputNameCounter),
                INPUT_NAME_SIZE + std::to_string(inputNameCounter));
    }

    std::pair<std::string, std::string> getNextInputName() {
        inputNameCounter++;
        return getCurrentInputNamePair();
    }

    std::string getCurrentLLVMFuncName() {
        std::string res;
        res.append(LLVMFuncName).append(std::to_string(LLVMFuncNameCounter));
        return res;
    }

    std::string getNextLLVMFuncName() {
        LLVMFuncNameCounter++;
        return getCurrentLLVMFuncName();
    }

    std::string getCurrentJoinKeyName() {
        std::string res;
        res.append(JOIN_KEY_NAME).append(std::to_string(join_type_counter));
        return res;
    }

    std::string getCurrentJoinValue1Name() {
        std::string res;
        res.append(JOIN_VALUE1_NAME).append(std::to_string(join_type_counter));
        return res;
    }

    std::string getCurrentJoinValue2Name() {
        std::string res;
        res.append(JOIN_VALUE2_NAME).append(std::to_string(join_type_counter));
        return res;
    }

    std::string getCurrentReduceByKeyKeyName() {
        std::string res;
        res.append(REDUCE_BY_KEY_KEY_NAME)
                .append(std::to_string(reduce_by_key_type_counter));
        return res;
    }

    std::string getCurrentReduceByKeyValueName() {
        std::string res;
        res.append(REDUCE_BY_KEY_VALUE_NAME)
                .append(std::to_string(reduce_by_key_type_counter));
        return res;
    }

    std::string getCurrentTupleName() {
        std::string res;
        res.append(TUPLE_NAME).append(std::to_string(tupleCounter));
        return res;
    }

    std::string getNextTupleName() {
        tupleCounter++;
        return getCurrentTupleName();
    }

    std::string getCurrentOperatorName() {
        std::string res;
        res.append(OPERATOR_NAME).append(std::to_string(operatorCounter));
        return res;
    }

    std::string getNextOperatorName() {
        operatorCounter++;
        return getCurrentOperatorName();
    }

    /***
     * generate a struct type
     * returns the tuple type name(e.g tuple_0) and the number of fields in this
     * tuple
     */
    std::pair<std::string, size_t> generateTupleType(const std::string &type);

    void emitOperatorMake(const std::string &opClass, const DAGOperator *op,
                          const std::string &opVarName,
                          const std::string &opName = "",
                          const std::string &extraArgs = "",
                          bool add_boolean_template = false);
    void emitJoinMake(const std::string &opVarName, DAGJoin *op);
    void emitCartesianMake(const std::string &opVarName, DAGCartesian *op);
    void emitReduceByKeyMake(const std::string &opVarName, DAGReduceByKey *op,
                             const std::string &operatorName);
    void emitLLVMFunctionWrapper(DAGOperator *op, const std::string &opName,
                                 bool returnsBool = false);
    /**
     * functions which take two arguments of the same type (e.g. reduce)
     */
    void emitLLVMFunctionWrapperBinaryArgs(DAGOperator *op,
                                           const std::string &opName,
                                           const std::string &inputType,
                                           size_t inputSize,
                                           bool reduce_by_key = false);
    void emitFuncEnd(const std::string &action);

    /**
     * generate Key, Value1, Value2 struct types
     * for current join
     */
    void generateJoinKeyValueTypes(DAGJoin *op);
    /**
     * generate Key and Value struct types
     * for current reduce by key
     */
    void generateReduceByKeyKeyValueTypes(DAGReduceByKey *op);
    std::string genComment(const std::string &com);
    void emitComment(const std::string &opName);
    // cppcheck-suppress unusedPrivateFunction
    std::string parseTupleType(const std::string &type);
    void makeDirectory();
    void storeLLVMCode(const std::string &ir, const std::string &opName);

    void appendLineBody(const std::string &str) {
        body.append(std::string(tabInd, '\t')).append(str).append(";\n");
    }

    void appendLineBodyNoCol(const std::string &str = "") {
        body.append(std::string(tabInd, '\t')).append(str).append("\n");
    }

    void addGenIncludes() {
        //        includes.insert("<std::vector>");
    }

    std::string writeHeader() {
        return "/**\n"
               " * Auto-generated execution plan\n"
               " */\n";
    }

    //    std::string writeJoinTupleDefs() {
    //        std::string line = "";
    //        line += genComment("join key value definitions");
    //        for (auto tupleDef : joinKeyValuesTypeDefs) {
    //            line += tupleDef;
    //        }
    //        return line;
    //    }

    std::string writeFuncDecl();
    std::string writeIncludes();
    std::string writeTupleDefs(const std::string &action);
    std::string writeLLVMFuncDecls();
    void write_execute(const std::string &final_code);
    void write_c_execute(const std::string &action);
    void write_c_executeh(const std::string &action);
};

#endif  // CODE_GEN_CODEGENVISITOR_H
