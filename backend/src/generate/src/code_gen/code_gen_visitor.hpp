//
// Created by sabir on 21.07.17.
//

#ifndef CODE_GEN_CODE_GEN_VISITOR_HPP
#define CODE_GEN_CODE_GEN_VISITOR_HPP

#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "context.hpp"
#include "dag/collection/tuple.hpp"
#include "dag/dag.hpp"
#include "dag/operators/all_operator_declarations.hpp"
#include "utils/visitor.hpp"

namespace code_gen {
namespace cpp {

struct CodeGenVisitor
    : public Visitor<CodeGenVisitor, DAGOperator, dag::AllOperatorTypes> {
public:
    struct OperatorDesc {
        std::string var_name;
        const StructDef *return_type{};
    };

    using OperatorRegistry =
            std::unordered_map<const DAGOperator *, OperatorDesc>;

    CodeGenVisitor(const DAG *const dag, Context *const context,
                   std::ostream &plan_body)
        : dag_(dag), context_(context), plan_body_(plan_body) {}

    /*
     * Implementation of Visitor interface
     */
    void operator()(DAGAssertCorrectOpenNextClose *op);
    void operator()(DAGConstantTuple *op);
    void operator()(DAGColumnScan *op);
    void operator()(DAGGroupBy *op);
    void operator()(DAGMap *op);
    void operator()(DAGMaterializeColumnChunks *op);
    void operator()(DAGMaterializeParquet *op);
    void operator()(DAGMaterializeRowVector *op);
    void operator()(DAGParallelMapOmp *op);
    void operator()(DAGParameterLookup *op);
    void operator()(DAGParquetScan *op);
    void operator()(DAGPartition *op);
    void operator()(DAGPipeline *op);
    void operator()(DAGProjection *op);
    void operator()(DAGReduce *op);
    void operator()(DAGRange *op);
    void operator()(DAGEnsureSingleTuple *op);
    void operator()(DAGExpandPattern *op);
    void operator()(DAGFilter *op);
    void operator()(DAGJoin *op);
    void operator()(DAGCartesian *op);
    void operator()(DAGReduceByKey *op);
    void operator()(DAGReduceByKeyGrouped *op);
    void operator()(DAGRowScan *op);
    void operator()(DAGSplitColumnData *op);
    void operator()(DAGSplitRange *op);
    void operator()(DAGSplitRowData *op);
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
    void emitOperatorMake(
            const std::string &variable_name, const std::string &operator_name,
            const DAGOperator *op,
            const std::vector<std::string> &extra_template_args = {},
            const std::vector<std::string> &extra_args = {});
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CODE_GEN_VISITOR_HPP