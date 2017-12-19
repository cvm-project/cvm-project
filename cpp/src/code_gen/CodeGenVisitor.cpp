//
// Created by sabir on 21.07.17.
//

#include "CodeGenVisitor.h"

#include <cstdlib>
#include <fstream>
#include <regex>
#include <set>
#include <unordered_map>
#include <utility>

#include <sys/stat.h>

#include "config.h"
#include "dag/DAG.h"
#include "dag/DAGOperators.h"
#include "utils/utils.h"

void CodeGenVisitor::start_visit(DAG *dag) {
    makeDirectory();
    addGenIncludes();
    tabInd++;

    this->visitDag(dag);

    emitFuncEnd();

    std::string final_code;
    final_code += writeHeader() + "\n" + writeIncludes() + "\n" +
                  writeTupleDefs() + "\n" + writeLLVMFuncDecls() + "\n" +
                  writeFuncDecl() + "\n" + body;

    write_execute(final_code);

    write_executeh();
}

void CodeGenVisitor::visit(DAGCollection *op) {
    std::string operatorName = "CollectionSourceOperator";
    emitComment(operatorName);

    std::string opName = getNextOperatorName();
    auto tType = generateTupleType(op->output_type);
    operatorNameTupleTypeMap.emplace(
            op->id, std::make_tuple(opName, tType.first, tType.second));

    includes.insert("\"operators/" + operatorName + ".h\"");
    std::pair<std::string, std::string> inputNamePair = getNextInputName();
    emitOperatorMake(operatorName, op, opName, "",
                     "(" + tType.first + "*)" + inputNamePair.first + ", " +
                             inputNamePair.second,
                     op->add_index);
    inputNames.push_back(inputNamePair);
    appendLineBodyNoCol();
}

void CodeGenVisitor::visit(DAGMap *op) {
    std::string operatorName = "MapOperator";
    emitComment(operatorName);

    std::string opName = getNextOperatorName();

    auto tType = generateTupleType(op->output_type);
    operatorNameTupleTypeMap.emplace(
            op->id, std::make_tuple(opName, tType.first, tType.second));

    includes.insert("\"operators/" + operatorName + ".h\"");

    storeLLVMCode(op->llvm_ir, "map");
    emitLLVMFunctionWrapper(op, "map");

    emitOperatorMake(operatorName, op, opName, "map");
    appendLineBodyNoCol();
}

void CodeGenVisitor::visit(DAGReduce *op) {
    std::string operatorName = "ReduceOperator";

    emitComment(operatorName);

    std::string opName = getNextOperatorName();

    auto tType = std::get<1>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
    auto typeSize =
            std::get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
    operatorNameTupleTypeMap.emplace(op->id,
                                     std::make_tuple(opName, tType, typeSize));

    includes.insert("\"operators/" + operatorName + ".h\"");

    storeLLVMCode(op->llvm_ir, "reduce");
    emitLLVMFunctionWrapperBinaryArgs(
            op, "reduce",
            std::get<1>(operatorNameTupleTypeMap[op->predecessors[0]->id]),
            std::get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]));

    emitOperatorMake(operatorName, op, opName, "reduce");
    appendLineBodyNoCol();
}

void CodeGenVisitor::visit(DAGRange *op) {
    std::string operatorName = "RangeSourceOperator";

    emitComment(operatorName);

    std::string opName = getNextOperatorName();
    auto tType = generateTupleType(op->output_type);
    operatorNameTupleTypeMap.emplace(
            op->id, std::make_tuple(opName, tType.first, tType.second));

    includes.insert("\"operators/" + operatorName + ".h\"");

    std::string args = op->from + ", " + op->to + ", " + op->step;
    emitOperatorMake(operatorName, op, opName, "", args);
    appendLineBodyNoCol();
};

void CodeGenVisitor::visit(DAGFilter *op) {
    std::string operatorName = "FilterOperator";

    emitComment(operatorName);

    std::string opName = getNextOperatorName();
    auto tType = operatorNameTupleTypeMap[op->predecessors[0]->id];
    operatorNameTupleTypeMap.emplace(
            op->id,
            std::make_tuple(opName, std::get<1>(tType), std::get<2>(tType)));

    includes.insert("\"operators/" + operatorName + ".h\"");

    storeLLVMCode(op->llvm_ir, "filter");
    emitLLVMFunctionWrapper(op, "filter", true);

    emitOperatorMake(operatorName, op, opName, "filter");
    appendLineBodyNoCol();
};

void CodeGenVisitor::visit(DAGJoin *op) {
    std::string operatorName = "JoinOperator";
    emitComment(operatorName);

    std::string opName = getNextOperatorName();
    auto tType = generateTupleType(op->output_type);
    operatorNameTupleTypeMap.emplace(
            op->id, std::make_tuple(opName, tType.first, tType.second));

    // add join k1, v1, v2 tuple types
    join_type_counter++;

    generateJoinKeyValueTypes(op);

    includes.insert("\"operators/" + operatorName + ".h\"");

    emitJoinMake(opName, op);
    appendLineBodyNoCol();
};

void CodeGenVisitor::visit(DAGCartesian *op) {
    std::string operatorName = "CartesianOperator";
    emitComment(operatorName);

    std::string opName = getNextOperatorName();
    auto tType = generateTupleType(op->output_type);
    operatorNameTupleTypeMap.emplace(
            op->id, std::make_tuple(opName, tType.first, tType.second));

    includes.insert("\"operators/" + operatorName + ".h\"");

    emitCartesianMake(opName, op);
    appendLineBodyNoCol();
};

void CodeGenVisitor::visit(DAGReduceByKey *op) {
    bool is_grouped = op->fields[0].properties->find(FL_GROUPED) !=
                              op->fields[0].properties->end() ||
                      op->fields[0].properties->find(FL_SORTED) !=
                              op->fields[0].properties->end() ||
                      op->fields[0].properties->find(FL_UNIQUE) !=
                              op->fields[0].properties->end();
    //        is_grouped = 0;
    std::string operatorName =
            is_grouped ? "ReduceByKeyGroupedOperator" : "ReduceByKeyOperator";
    emitComment(operatorName);

    std::string opName = getNextOperatorName();
    // reuse the pred tuple type
    auto predNameTuple = operatorNameTupleTypeMap[op->predecessors[0]->id];
    operatorNameTupleTypeMap.emplace(
            op->id, std::make_tuple(opName, std::get<1>(predNameTuple),
                                    std::get<2>(predNameTuple)));

    // add reducebykey key and value tuple types
    reduce_by_key_type_counter++;

    generateReduceByKeyKeyValueTypes(op);

    storeLLVMCode(op->llvm_ir, "reduce_by_key");
    // assume the key is a scalar type
    emitLLVMFunctionWrapperBinaryArgs(
            op, "reduce_by_key", getCurrentReduceByKeyValueName(),
            std::get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]) - 1,
            true);

    includes.insert("\"operators/" + operatorName + ".h\"");

    emitReduceByKeyMake(opName, op, operatorName);
    appendLineBodyNoCol();
};

void CodeGenVisitor::emitOperatorMake(const std::string &opClass,
                                      const DAGOperator *const op,
                                      const std::string &opVarName,
                                      const std::string &opName,
                                      const std::string &extraArgs,
                                      bool add_boolean_template) {
    std::string line("auto ");
    line.append(opVarName).append(" = make").append(opClass).append("<").append(
            getCurrentTupleName());
    if (add_boolean_template) {
        line.append(", true");
    }
    line.append(">(");

    std::string argList;
    for (auto it = op->predecessors.begin(); it < op->predecessors.end();
         it++) {
        argList.append("&" + std::get<0>(operatorNameTupleTypeMap[(*it)->id]));
        if (it != (--op->predecessors.end())) {
            argList.append(", ");
        }
    }

    if (!opName.empty()) {
        if (!argList.empty()) {
            argList.append(", ");
        }
        argList.append(opName + snake_to_camel_string(getCurrentLLVMFuncName()))
                .append("()");
    }

    if (!extraArgs.empty()) {
        if (!argList.empty()) {
            argList.append(", ");
        }
        argList.append(extraArgs);
    }

    line.append(argList);
    line.append(")");
    appendLineBody(line);
}

void CodeGenVisitor::emitJoinMake(const std::string &opVarName, DAGJoin *op) {
    std::string line("auto ");
    line.append(opVarName)
            .append(" = make")
            .append("JoinOperator")
            .append("<")
            .append(getCurrentTupleName() + ", " + getCurrentJoinKeyName() +
                    ", " + getCurrentJoinValue1Name() + ", " +
                    getCurrentJoinValue2Name())
            .append(">(");

    std::string argList;
    for (auto it = op->predecessors.begin(); it < op->predecessors.end();
         it++) {
        argList.append("&" + std::get<0>(operatorNameTupleTypeMap[(*it)->id]));
        if (it != (--op->predecessors.end())) {
            argList.append(", ");
        }
    }

    line.append(argList);
    line.append(")");
    appendLineBody(line);
}

void CodeGenVisitor::emitCartesianMake(const std::string &opVarName,
                                       DAGCartesian *op) {
    std::string line("auto ");
    std::string pred2Tuple =
            std::get<1>(operatorNameTupleTypeMap[op->predecessors[1]->id]);
    line.append(opVarName)
            .append(" = make")
            .append("CartesianOperator")
            .append("<")
            .append(getCurrentTupleName() + ", " + pred2Tuple)
            .append(">(");

    std::string argList;
    for (auto it = op->predecessors.begin(); it < op->predecessors.end();
         it++) {
        argList.append("&" + std::get<0>(operatorNameTupleTypeMap[(*it)->id]));
        if (it != (--op->predecessors.end())) {
            argList.append(", ");
        }
    }

    line.append(argList);
    line.append(")");
    appendLineBody(line);
}

void CodeGenVisitor::emitReduceByKeyMake(const std::string &opVarName,
                                         DAGReduceByKey *op,
                                         const std::string &operatorName) {
    std::string line("auto ");
    line.append(opVarName)
            .append(" = make")
            .append(operatorName)
            .append("<")
            .append(getCurrentTupleName() + ", " +
                    getCurrentReduceByKeyKeyName() + ", " +
                    getCurrentReduceByKeyValueName())
            .append(">(");

    std::string argList;
    for (auto it = op->predecessors.begin(); it < op->predecessors.end();
         it++) {
        argList.append("&" + std::get<0>(operatorNameTupleTypeMap[(*it)->id]));
        argList.append(", ");
    }

    argList.append("reduceByKey" +
                   snake_to_camel_string(getCurrentLLVMFuncName()))
            .append("()");

    line.append(argList);
    line.append(")");
    appendLineBody(line);
}

/***
 * generate a struct type
 * returns the tuple type name(e.g tuple_0) and the number of fields in this
 * tuple
 */
std::pair<std::string, size_t> CodeGenVisitor::generateTupleType(
        const std::string &type) {
    std::string tName = getNextTupleName();
    std::string type_ = type;
    type_ = string_replace(type_, "(", "");
    type_ = string_replace(type_, ")", "");
    type_ = string_replace(type_, " ", "");
    std::vector<std::string> types = split_string(type_, ",");
    std::string line("typedef struct {\n");
    std::string varName = "v";
    for (size_t i = 0; i < types.size(); i++) {
        line += "\t" + types[i] + " " + varName + std::to_string(i) + "; ";
    }
    line += "} " + tName + ";\n\n";

    tupleTypeDefs.push_back(line);
    resultTypeDef = line;
    return std::make_pair(tName, types.size());
}

void CodeGenVisitor::emitLLVMFunctionWrapper(DAGOperator *op,
                                             const std::string &opName,
                                             bool returnsBool) {
    std::string llvmFuncName = opName + getCurrentLLVMFuncName();
    std::string className = snake_to_camel_string(llvmFuncName);
    appendLineBodyNoCol("class " + className + " {");
    appendLineBodyNoCol("public:");
    tabInd++;
    std::string inputType =
            std::get<1>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
    std::string retType = std::get<1>(operatorNameTupleTypeMap[op->id]);
    appendLineBodyNoCol(
            std::string("auto operator()(").append(inputType).append(" t) {"));
    tabInd++;
    std::string args;
    std::string varName = "v";
    for (size_t i = 0;
         i < std::get<2>(operatorNameTupleTypeMap[op->predecessors[0]->id]);
         i++) {
        args += "t." + varName + std::to_string(i) + ", ";
    }
    if (!args.empty()) {
        args.pop_back();
        args.pop_back();
    }
    appendLineBody("return " + llvmFuncName + "(" + args + ")");
    tabInd--;
    appendLineBodyNoCol("}");
    tabInd--;
    appendLineBodyNoCol("};");

    std::string flatInputType =
            string_replace(op->predecessors[0]->output_type, "(", "");
    flatInputType = string_replace(flatInputType, ")", "");
    if (returnsBool) {
        llvmFuncDecls.push_back("bool " + llvmFuncName + "(" + flatInputType +
                                ");");
    } else {
        llvmFuncDecls.push_back(retType + " " + llvmFuncName + "(" +
                                flatInputType + ");");
    }
}

/**
 * functions which take two arguments of the same type (e.g. reduce)
 */
void CodeGenVisitor::emitLLVMFunctionWrapperBinaryArgs(
        DAGOperator *op, const std::string &opName,
        const std::string &inputType, size_t inputSize, bool reduce_by_key) {
    std::string llvmFuncName = opName + getCurrentLLVMFuncName();
    std::string className = snake_to_camel_string(llvmFuncName);
    appendLineBodyNoCol("class " + className + " {");
    appendLineBodyNoCol("public:");
    tabInd++;
    std::string retType = std::get<1>(operatorNameTupleTypeMap[op->id]);
    appendLineBodyNoCol(std::string("auto operator()(")
                                .append(inputType + " t0, ")
                                .append(inputType + " t1) {"));
    tabInd++;
    std::string args;
    std::string varName = "v";
    for (size_t i = 0; i < inputSize; i++) {
        args += "t0." + varName + std::to_string(i) + ", ";
    }

    for (size_t i = 0; i < inputSize; i++) {
        args += "t1." + varName + std::to_string(i) + ", ";
    }

    if (!args.empty()) {
        args.pop_back();
        args.pop_back();
    }
    appendLineBody("return " + llvmFuncName + "(" + args + ")");
    tabInd--;
    appendLineBodyNoCol("}");
    tabInd--;
    appendLineBodyNoCol("};");
    if (reduce_by_key) {
        std::string flatInputType;
        for (size_t i = 1; i < op->predecessors[0]->fields.size(); i++) {
            auto t = op->predecessors[0]->fields[i];
            flatInputType += t.type + ", ";
        }
        flatInputType.pop_back();
        flatInputType.pop_back();
        llvmFuncDecls.push_back(inputType + " " + llvmFuncName + "(" +
                                flatInputType + ", " + flatInputType + ");");
    } else {
        std::string flatInputType =
                string_replace(op->predecessors[0]->output_type, "(", "");
        flatInputType = string_replace(flatInputType, ")", "");
        llvmFuncDecls.push_back(retType + " " + llvmFuncName + "(" +
                                flatInputType + ", " + flatInputType + ");");
    }
}

void CodeGenVisitor::emitFuncEnd() {
    emitComment("collecting the result");
    appendLineBody(getCurrentOperatorName() + ".open()");
    appendLineBody("size_t allocatedSize = 2");
    appendLineBody("size_t resSize = 0");
    appendLineBody(getCurrentTupleName() + " *result = (" +
                   getCurrentTupleName() + " *) malloc(sizeof(" +
                   getCurrentTupleName() + ") * allocatedSize)");
    appendLineBodyNoCol("while (auto res = " + getCurrentOperatorName() +
                        ".next()) {");
    tabInd++;
    appendLineBodyNoCol("if (allocatedSize <= resSize) {");
    tabInd++;
    appendLineBody("allocatedSize *= 2");
    appendLineBody("result = (" + getCurrentTupleName() +
                   "*) realloc(result, sizeof(" + getCurrentTupleName() +
                   ") * allocatedSize)");
    tabInd--;
    appendLineBodyNoCol("}");
    appendLineBody("result[resSize] = res.value");
    appendLineBody("resSize++");
    tabInd--;
    appendLineBodyNoCol("}");
    appendLineBody(getCurrentOperatorName() + ".close()");
    appendLineBody(
            "result_type ret = (result_type) "
            "malloc(sizeof(result_struct))");
    appendLineBody("ret->data = result");
    appendLineBody("ret->size = resSize");
    appendLineBody("return ret");

    tabInd--;
    appendLineBodyNoCol("}");

    appendLineBodyNoCol(
            "void free_result(result_type ptr) {\n"
            "    if (ptr != NULL && ptr->data != NULL) {\n"
            "        free(ptr->data);\n"
            "        ptr->data = NULL;\n"
            "    }\n"
            "    if (ptr != NULL) {\n"
            "        free(ptr);\n"
            "    }\n"
            "    ptr = NULL;\n"
            "}");
    appendLineBodyNoCol("}");
}

/**
 * generate Key, Value1, Value2 struct types
 * for current join
 */
void CodeGenVisitor::generateJoinKeyValueTypes(DAGJoin *op) {
    std::string up1Type = op->predecessors[0]->output_type;
    std::string up2Type = op->predecessors[1]->output_type;

    // key1 type
    std::string keyName = getCurrentJoinKeyName();
    std::vector<std::string> keyTypes;
    if (up1Type.find("((") == 0) {
        // key is a tuple
        std::regex reg("((.*)");
        std::smatch m;
        std::regex_search(up1Type, m, reg);
        std::string typesStr;
        for (auto x : m) typesStr = x;
        typesStr = string_replace(typesStr, "(", " ");
        typesStr = string_replace(typesStr, ")", " ");
        typesStr = string_replace(typesStr, " ", "");
        keyTypes = split_string(typesStr, ",");
    } else {
        // key is the first element
        std::string typesStr = string_replace(up1Type, "(", " ");
        typesStr = string_replace(typesStr, ")", " ");
        typesStr = string_replace(typesStr, " ", "");
        keyTypes.push_back(split_string(typesStr, ",")[0]);
    }
    std::string line("typedef struct {\n");
    std::string varName = "v";
    for (size_t i = 0; i < keyTypes.size(); i++) {
        line += "\t" + keyTypes[i] + " " + varName + std::to_string(i) + "; ";
    }
    line += "} " + keyName + ";\n\n";

    // up1 type
    std::vector<std::string> up1Types;
    if (up1Type.find("((") == 0) {
        // assume for now the key is a scalar type
    } else {
        std::string typesStr = string_replace(up1Type, "(", " ");
        typesStr = string_replace(typesStr, ")", " ");
        typesStr = string_replace(typesStr, " ", "");
        up1Types = split_string(typesStr, ",");
        up1Types =
                std::vector<std::string>(up1Types.begin() + 1, up1Types.end());
        line += "typedef struct {\n";

        for (size_t i = 0; i < up1Types.size(); i++) {
            line += "\t" + up1Types[i] + " " + varName + std::to_string(i) +
                    "; ";
        }
        line += "} " + getCurrentJoinValue1Name() + ";\n\n";
    }

    // up2 type
    std::vector<std::string> up2Types;
    if (up2Type.find("((") == 0) {
        // assume for now the key is a scalar type
    } else {
        std::string typesStr = string_replace(up2Type, "(", " ");
        typesStr = string_replace(typesStr, ")", " ");
        typesStr = string_replace(typesStr, " ", "");
        up2Types = split_string(typesStr, ",");
        up2Types =
                std::vector<std::string>(up2Types.begin() + 1, up2Types.end());
        line += "typedef struct {\n";

        for (size_t i = 0; i < up2Types.size(); i++) {
            line += "\t" + up2Types[i] + " " + varName + std::to_string(i) +
                    "; ";
        }
        line += "} " + getCurrentJoinValue2Name() + ";\n\n";
    }

    tupleTypeDefs.push_back(line);
}

/**
 * generate Key and Value struct types
 * for current reduce by key
 */
void CodeGenVisitor::generateReduceByKeyKeyValueTypes(DAGReduceByKey *op) {
    std::string upType = op->predecessors[0]->output_type;

    // key type
    std::string keyName = getCurrentReduceByKeyKeyName();
    std::vector<std::string> keyTypes;
    //        if (upType.find("((") == 0) {
    //            //key is a tuple
    //            std::regex reg("((.*)");
    //            std::smatch m;
    //            std::regex_search(upType, m, reg);
    //            std::string typesStr;
    //            for (auto x:m) typesStr = x;
    //            typesStr = string_replace(typesStr, "(", " ");
    //            typesStr = string_replace(typesStr, ")", " ");
    //            typesStr = string_replace(typesStr, " ", "");
    //            keyTypes = split_string(typesStr, ",");
    //        }
    //        else {
    // key is the first element
    std::string typesStr = string_replace(upType, "(", " ");
    typesStr = string_replace(typesStr, ")", " ");
    typesStr = string_replace(typesStr, " ", "");
    keyTypes.push_back(split_string(typesStr, ",")[0]);
    //        }
    std::string line("typedef struct {\n");
    std::string varName = "v";
    for (size_t i = 0; i < keyTypes.size(); i++) {
        line += "\t" + keyTypes[i] + " " + varName + std::to_string(i) + "; ";
    }
    line += "} " + keyName + ";\n\n";

    // upstream type
    std::vector<std::string> upTypes;
    if (upType.find("((") == 0) {
        // assume for now the key is a scalar type
    } else {
        std::string typesStr = string_replace(upType, "(", " ");
        typesStr = string_replace(typesStr, ")", " ");
        typesStr = string_replace(typesStr, " ", "");
        upTypes = split_string(typesStr, ",");
        upTypes = std::vector<std::string>(upTypes.begin() + 1, upTypes.end());
        line += "typedef struct {\n";

        for (size_t i = 0; i < upTypes.size(); i++) {
            line += "\t" + upTypes[i] + " " + varName + std::to_string(i) +
                    "; ";
        }
        line += "} " + getCurrentReduceByKeyValueName() + ";\n\n";
    }

    tupleTypeDefs.push_back(line);
}

std::string CodeGenVisitor::genComment(const std::string &com) {
    return "\n/**" + com + "**/\n";
}

void CodeGenVisitor::emitComment(const std::string &opName) {
    tabInd++;
    appendLineBodyNoCol(genComment(opName));
    tabInd--;
}

// cppcheck-suppress unusedPrivateFunction
std::string CodeGenVisitor::parseTupleType(const std::string &type) {
    std::string ret = type;
    ret = string_replace(ret, "(", "tuple<");
    ret = string_replace(ret, ")", ">");
    return ret;
}

void CodeGenVisitor::makeDirectory() {
    if (DUMP_FILES) {
        const int dirErr =
                system(("mkdir -p " + genDir + LLVM_FUNC_DIR).c_str());
        if (0 != dirErr) {
            std::cerr << ("Error creating gen directory!") << std::endl;
            std::exit(1);
        }
    }
}

void CodeGenVisitor::storeLLVMCode(const std::string &ir,
                                   const std::string &opName) {
    std::string patched_ir = ir;
    // the local_unnamed_addr std::string is not llvm-3.7 compatible:
    std::regex reg1("local_unnamed_addr #.? ");
    patched_ir = std::regex_replace(patched_ir, reg1, "");
    //        patched_ir = string_replace(patched_ir, "local_unnamed_addr #1
    //        ", "");
    // replace the func name with our
    std::string funcName = opName + getNextLLVMFuncName();
    std::regex reg("@cfuncnotuniquename");
    patched_ir = std::regex_replace(patched_ir, reg, "@\"" + funcName + "\"");
    // write code to the gen dir
    if (DUMP_FILES) {
        std::string path = genDir + LLVM_FUNC_DIR + funcName + ".ll";
        std::ofstream out(path);
        out << patched_ir;
        out.close();
    }
}

std::string CodeGenVisitor::writeFuncDecl() {
    executeFuncParams = "";
    for (auto inputPair : inputNames) {
        executeFuncParams += "void *" + inputPair.first + ", unsigned long " +
                             inputPair.second + ", ";
    }
    if (!executeFuncParams.empty()) {
        executeFuncParams.pop_back();
        executeFuncParams.pop_back();
    }
    return "extern \"C\" { " + executeFuncReturn + " execute(" +
           executeFuncParams + ") {";
}

std::string CodeGenVisitor::writeIncludes() {
    std::string ret;
    for (const auto &incl : includes) {
        ret.append("#include ").append(incl).append("\n");
    }
    ret.append("\n");
    return ret;
}

std::string CodeGenVisitor::writeTupleDefs() {
    std::string ret;
    std::string resultWrapper;
    ret.append(genComment("tuple definitions"));
    size_t len = tupleTypeDefs.size();
    for (size_t i = 0; i < len; i++) {
        ret.append(tupleTypeDefs[i]);
    }
    resultWrapper =
            "\ntypedef struct {\n "
            "\tunsigned long size;\n"
            "\t" +
            getCurrentTupleName() +
            " *data;\n"
            "} result_struct;\n"
            "typedef result_struct* result_type;\n";
    resultTypeDef += resultWrapper;
    return ret + resultWrapper;
}

std::string CodeGenVisitor::writeLLVMFuncDecls() {
    std::string ret;
    ret.append("extern \"C\" {\n");
    tabInd++;
    for (const auto &def : llvmFuncDecls) {
        ret.append(def + "\n");
    }
    tabInd--;
    ret.append("}\n");
    return ret;
}

void CodeGenVisitor::write_execute(const std::string &final_code) {
    std::ofstream out(genDir + "execute.cpp");
    out << final_code;
    out.close();
}

void CodeGenVisitor::write_executeh() {
    std::ofstream out(genDir + "execute.h");
    out << resultTypeDef
        << "\n"
           "" + executeFuncReturn +
                    " execute(" + executeFuncParams + ");\n";
    out << "\n"
           "void free_result(" +
                    executeFuncReturn + ");";
    out.close();
}
