#include "apply_visitor.hpp"

#include <type_traits>

#include <boost/function_output_iterator.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include "dag/DAG.h"
#include "dag/DAGOperator.h"

namespace dag {
namespace utils {

template <bool kReverseGraph, class DagType>
void ApplyInTopologicalOrderImpl(DagType *dag, const DagVisitor &visitor) {
    static_assert(std::is_same<std::remove_const<DAG>::type, DAG>::value,
                  "Must be instaniated with DAG or const DAG");

    const auto graph = kReverseGraph ? boost::make_reverse_graph(dag->graph())
                                     : dag->graph();
    boost::topological_sort(
            graph, boost::make_function_output_iterator([&](const auto v) {
                const auto op = dag->to_operator(v);
                visitor(op);
            }));
}

void ApplyInTopologicalOrder(DAG *const dag, const DagVisitor &visitor) {
    ApplyInTopologicalOrderImpl<false>(dag, visitor);
}

void ApplyInTopologicalOrder(const DAG *const dag, const DagVisitor &visitor) {
    ApplyInTopologicalOrderImpl<false>(dag, visitor);
}

void ApplyInReverseTopologicalOrder(DAG *const dag, const DagVisitor &visitor) {
    ApplyInTopologicalOrderImpl<true>(dag, visitor);
}

void ApplyInReverseTopologicalOrder(const DAG *const dag,
                                    const DagVisitor &visitor) {
    ApplyInTopologicalOrderImpl<true>(dag, visitor);
}

}  // namespace utils
}  // namespace dag
