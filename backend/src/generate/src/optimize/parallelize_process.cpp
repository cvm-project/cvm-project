#include "parallelize_process.hpp"

#include <boost/mpl/list.hpp>

#include "dag/dag.hpp"
#include "dag/operators/concurrent_execute.hpp"
#include "dag/operators/concurrent_execute_process.hpp"
#include "dag/operators/constant_tuple.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/visitor.hpp"

namespace optimize {

class CollectParallelMapsVisitor
    : public Visitor<CollectParallelMapsVisitor, const DAGOperator,
                     boost::mpl::list<             //
                             DAGConcurrentExecute  //
                             >::type> {
public:
    CollectParallelMapsVisitor(
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
                    "   runtime::operators::process_num_workers())";
            } else if (v == "$WID") {
                v = "static_cast<long>("
                    "   runtime::operators::process_worker_id())";
            }
        }
    }
};

void ParallelizeProcess::Run(DAG *const dag,
                             const std::string & /*config*/) const {
    // Specialize parallel maps
    std::vector<std::pair<DAG *, const DAGOperator *>> parallel_map_ops;
    dag::utils::ApplyInTopologicalOrder(dag, [&](const DAGOperator *const op) {
        CollectParallelMapsVisitor visitor(dag, &parallel_map_ops);
        visitor.Visit(op);
    });
    for (auto [dag, op] : parallel_map_ops) {
        dag->ReplaceOperator(op, new DAGConcurrentExecuteProcess());
    }

    // Specialize degree of parallelism
    dag::utils::ApplyInTopologicalOrderRecursively(
            dag, [&](DAGOperator *const op, DAG * /*dag*/) {
                SpecializeDegreeOfParallelismVisitor visitor;
                visitor.Visit(op);
            });
}

}  // namespace optimize
