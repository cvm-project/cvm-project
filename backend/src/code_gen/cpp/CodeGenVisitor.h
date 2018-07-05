//
// Created by sabir on 21.07.17.
//

#ifndef CODE_GEN_CPP_CODEGENVISITOR_H
#define CODE_GEN_CPP_CODEGENVISITOR_H

#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string/join.hpp>

#include "context.hpp"
#include "dag/DAG.h"
#include "dag/all_operator_declarations.hpp"
#include "dag/collection/tuple.hpp"
#include "utils/visitor.hpp"

namespace code_gen {
namespace cpp {

struct CodeGenVisitor
    : public Visitor<CodeGenVisitor, DAGOperator, dag::AllOperatorTypes> {
public:
    struct OperatorDesc {
        size_t id{};
        std::string var_name;
        const StructDef *return_type{};
    };

    using OperatorRegistry = std::unordered_map<size_t, OperatorDesc>;

    CodeGenVisitor(const DAG *const dag, Context *const context,
                   std::ostream &plan_body)
        : dag_(dag), context_(context), plan_body_(plan_body) {}

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
    void operator()(DAGEnsureSingleTuple *op);
    void operator()(DAGFilter *op);
    void operator()(DAGJoin *op);
    void operator()(DAGCartesian *op);
    void operator()(DAGReduceByKey *op);
    void operator()(DAGReduceByKeyGrouped *op);
    void operator()(DAGOperator *op);

    /*
     * Members
     */
    const DAG *const dag_;
    Context *const context_;
    std::ostream &plan_body_;
    OperatorRegistry operator_descs_{};

private:
    /*
     * Implementation helpers for visit functions
     */
    std::string visit_common(DAGOperator *op, const std::string &operator_name);
    void visit_reduce_by_key(DAGOperator *op, const std::string &operator_name);
    std::string visitLLVMFunc(const DAGOperator &op,
                              const std::vector<const StructDef *> &input_types,
                              const std::string &return_type);
    void emitOperatorMake(
            const std::string &variable_name, const std::string &operator_name,
            const DAGOperator *op,
            const std::vector<std::string> &extra_template_args = {},
            const std::vector<std::string> &extra_args = {});
    void emitLLVMFunctionWrapper(
            const std::string &func_name,
            const std::vector<const StructDef *> &input_types,
            const std::string &return_type);
    void storeLLVMCode(const std::string &ir, const std::string &func_name);

    /*
     * Manage tuple/function/variable names
     */
    const StructDef *EmitStructDefinition(
            const dag::type::Type *key, const std::vector<std::string> &types,
            const std::vector<std::string> &names);
    const StructDef *EmitTupleStructDefinition(const dag::type::Tuple *tuple);
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_CODEGENVISITOR_H