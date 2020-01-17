#include "parallelize_omp.hpp"

#include <boost/mpl/list.hpp>

#include "dag/dag.hpp"
#include "dag/operators/constant_tuple.hpp"
#include "dag/operators/parallel_map.hpp"
#include "dag/operators/parallel_map_omp.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/visitor.hpp"

namespace optimize {
namespace parallelize_omp {

class CollectParallelMapsVisitor
    : public Visitor<CollectParallelMapsVisitor, const DAGOperator,
                     boost::mpl::list<       //
                             DAGParallelMap  //
                             >::type> {
public:
    CollectParallelMapsVisitor(
            DAG *const dag, std::vector<std::pair<DAG *, const DAGOperator *>>
                                    *const parallel_map_ops)
        : dag_(dag), parallel_map_ops_(parallel_map_ops) {}

    void operator()(const DAGParallelMap *const op) {
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
                v = "omp_get_num_threads()";
            }
        }
    }
};

}  // namespace parallelize_omp

void ParallelizeOmp::Run(DAG *const dag, const std::string & /*config*/) const {
    // Specialize parallel maps
    std::vector<std::pair<DAG *, const DAGOperator *>> parallel_map_ops;
    dag::utils::ApplyInTopologicalOrderRecursively(
            dag, [&](const DAGOperator *const op, DAG *const dag) {
                parallelize_omp::CollectParallelMapsVisitor visitor(
                        dag, &parallel_map_ops);
                visitor.Visit(op);
            });
    for (auto [dag, op] : parallel_map_ops) {
        dag->ReplaceOperator(op, new DAGParallelMapOmp());
    }

    // Specialize degree of parallelism
    dag::utils::ApplyInTopologicalOrderRecursively(
            dag, [&](DAGOperator *const op, DAG *const /*dag*/) {
                parallelize_omp::SpecializeDegreeOfParallelismVisitor visitor;
                visitor.Visit(op);
            });
}

}  // namespace optimize
