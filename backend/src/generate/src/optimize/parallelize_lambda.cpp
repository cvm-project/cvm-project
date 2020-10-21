#include "parallelize_lambda.hpp"

#include <boost/mpl/list.hpp>

#include "dag/dag.hpp"
#include "dag/operators/concurrent_execute.hpp"
#include "dag/operators/concurrent_execute_lambda.hpp"
#include "dag/operators/constant_tuple.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/visitor.hpp"

namespace optimize {
namespace parallelize_lambda {

class CollectConcurrentExecuteVisitor
    : public Visitor<CollectConcurrentExecuteVisitor, const DAGOperator,
                     boost::mpl::list<             //
                             DAGConcurrentExecute  //
                             >::type> {
public:
    CollectConcurrentExecuteVisitor(
            DAG *const dag, std::vector<std::pair<DAG *, const DAGOperator *>>
                                    *const parallel_map_ops)
        : dag_(dag), parallel_map_ops_(parallel_map_ops) {}

    void operator()(const DAGConcurrentExecute *const op) {
        parallel_map_ops_->emplace_back(dag_, op);
    }

private:
    DAG *const dag_;
    std::vector<std::pair<DAG *, const DAGOperator *>> *const parallel_map_ops_;
};

class SpecializeDegreeOfParallelismVisitor
    : public Visitor<SpecializeDegreeOfParallelismVisitor, DAGOperator,
                     boost::mpl::list<         //
                             DAGConstantTuple  //
                             >::type> {
public:
    void operator()(DAGConstantTuple *const op) {
        for (auto &v : op->values) {
            if (v == "$DOP") {
                v = "static_cast<long>("
                    "   runtime::operators::lambda_num_workers())";
            } else if (v == "$WID") {
                v = "static_cast<long>("
                    "   runtime::operators::lambda_worker_id())";
            }
        }
    }
};

}  // namespace parallelize_lambda

void ParallelizeLambda::Run(DAG *const dag,
                            const std::string & /*config*/) const {
    // Specialize parallel maps
    std::vector<std::pair<DAG *, const DAGOperator *>> parallel_map_ops;
    dag::utils::ApplyInTopologicalOrder(dag, [&](const DAGOperator *const op) {
        parallelize_lambda::CollectConcurrentExecuteVisitor visitor(
                dag, &parallel_map_ops);
        visitor.Visit(op);
    });
    for (auto [dag, op] : parallel_map_ops) {
        dag->ReplaceOperator(op, new DAGConcurrentExecuteLambda());
    }

    // Specialize degree of parallelism
    dag::utils::ApplyInTopologicalOrderRecursively(
            dag, [&](DAGOperator *const op, DAG * /*dag*/) {
                parallelize_lambda::SpecializeDegreeOfParallelismVisitor
                        visitor;
                visitor.Visit(op);
            });
}

}  // namespace optimize
