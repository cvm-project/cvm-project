//
// Created by sabir on 21.07.17.
//

#ifndef CODE_GEN_CPP_CODEGENVISITOR_H
#define CODE_GEN_CPP_CODEGENVISITOR_H

#include <cstdlib>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string/join.hpp>

#include "dag/DAG.h"
#include "dag/all_operator_declarations.hpp"
#include "dag/collection/tuple.hpp"
#include "utils/visitor.hpp"

namespace code_gen {
namespace cpp {

struct CodeGenVisitor
    : public Visitor<CodeGenVisitor, DAGOperator, dag::AllOperatorTypes> {
public:
    struct TupleTypeDesc {
        std::string name;
        std::vector<std::string> types;
        std::vector<std::string> names;
        static TupleTypeDesc MakeFromCollection(
                std::string &&name, const dag::collection::Tuple &tuple);
        std::string field_definitions() const;
        size_t num_fields() const {
            assert(types.size() == names.size());
            return types.size();
        }
        TupleTypeDesc computeHeadType(size_t head_size = 1) const;
        TupleTypeDesc computeTailType(size_t head_size = 1) const;
    };

    struct OperatorDesc {
        size_t id{};
        std::string var_name;
        TupleTypeDesc return_type;
    };

    CodeGenVisitor(const DAG *const dag, std::ostream &planBody,
                   std::ostream &planDeclarations, std::ostream &llvmCode)
        : dag_(dag),
          planBody_(planBody),
          planDeclarations_(planDeclarations),
          llvmCode_(llvmCode) {}

    /*
     * Implementation of Visitor interface
     */
    void operator()(DAGCollection *op);
    void operator()(DAGConstantTuple *op);
    void operator()(DAGMap *op);
    void operator()(DAGMaterializeRowVector *op);
    void operator()(DAGParameterLookup *op);
    void operator()(DAGReduce *op);
    void operator()(DAGRange *op);
    void operator()(DAGFilter *op);
    void operator()(DAGJoin *op);
    void operator()(DAGCartesian *op);
    void operator()(DAGReduceByKey *op);
    void operator()(DAGReduceByKeyGrouped *op);
    void operator()(DAGOperator *op);

    /*
     * These members are the "result" of the visitor
     */
    const DAG *const dag_;
    std::ostream &planBody_;
    std::ostream &planDeclarations_;
    std::ostream &llvmCode_;
    std::vector<std::pair<std::string, std::string>> inputNames;
    std::set<std::string> includes;
    std::unordered_map<std::string, std::string> tupleDefinitions_;
    std::unordered_map<size_t, OperatorDesc> operatorNameTupleTypeMap;

private:
    /*
     * Implementation helpers for visit functions
     */
    std::string visit_common(DAGOperator *op, const std::string &operator_name);
    void visit_reduce_by_key(DAGOperator *op, const std::string &operator_name);
    std::string visitLLVMFunc(const DAGOperator &op,
                              const std::vector<TupleTypeDesc> &input_types,
                              const std::string &return_type);
    std::string emitTupleDefinition(const TupleTypeDesc &type);
    void emitOperatorMake(
            const std::string &variable_name, const std::string &operator_name,
            const DAGOperator *op,
            const std::vector<std::string> &extra_template_args = {},
            const std::vector<std::string> &extra_args = {});
    void emitLLVMFunctionWrapper(const std::string &func_name,
                                 const std::vector<TupleTypeDesc> &input_types,
                                 const std::string &return_type);
    void storeLLVMCode(const std::string &ir, const std::string &func_name);

    /*
     * Manage tuple/function/variable names
     */
    std::unordered_map<std::string, size_t> unique_counters;

    size_t unique_counter(const std::string &name) const {
        return unique_counters.at(name);
    }

    void incrementUniqueCounter(const std::string &name) {
        auto it = unique_counters.emplace(name, -1).first;
        (it->second)++;
    }

    std::string unique_name(const std::string &name) const {
        return name + "_" + std::to_string(unique_counter(name));
    }

    auto getNextInputName() {
        incrementUniqueCounter("input");
        incrementUniqueCounter("input_size");
        return std::make_pair(unique_name("input"), unique_name("input_size"));
    }

    std::string getNextLLVMFuncName() {
        incrementUniqueCounter("_operator_function");
        return unique_name("_operator_function");
    }

    std::string getNextTupleName() {
        incrementUniqueCounter("tuple");
        return unique_name("tuple");
    }

    std::string getNextOperatorName() {
        incrementUniqueCounter("op");
        return unique_name("op");
    }
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_CODEGENVISITOR_H