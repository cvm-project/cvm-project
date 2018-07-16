#ifndef DAG_UTILS_APPLY_VISITOR_HPP
#define DAG_UTILS_APPLY_VISITOR_HPP

#include <functional>

class DAG;
class DAGOperator;

namespace dag {
namespace utils {

using DagVisitor = std::function<void(DAGOperator *)>;
using RecursiveDagVisitor = std::function<void(DAGOperator *, DAG *)>;
using RecursiveConstDagVisitor =
        std::function<void(DAGOperator *, const DAG *)>;

void ApplyInTopologicalOrder(DAG *dag, const DagVisitor &visitor);
void ApplyInTopologicalOrder(const DAG *dag, const DagVisitor &visitor);
void ApplyInTopologicalOrderRecursively(
        DAG *dag, const RecursiveDagVisitor &on_entry_visitor,
        const RecursiveDagVisitor &on_exit_visitor = [](auto /*op*/,
                                                        auto /*dag*/) {});
void ApplyInTopologicalOrderRecursively(
        const DAG *dag, const RecursiveConstDagVisitor &on_entry_visitor,
        const RecursiveConstDagVisitor &on_exit_visitor = [](auto /*op*/,
                                                             auto /*dag*/) {});

void ApplyInReverseTopologicalOrder(DAG *dag, const DagVisitor &visitor);
void ApplyInReverseTopologicalOrder(const DAG *dag, const DagVisitor &visitor);
void ApplyInReverseTopologicalOrderRecursively(
        DAG *dag, const RecursiveDagVisitor &on_entry_visitor,
        const RecursiveDagVisitor &on_exit_visitor = [](auto /*op*/,
                                                        auto /*dag*/) {});
void ApplyInReverseTopologicalOrderRecursively(
        const DAG *dag, const RecursiveConstDagVisitor &on_entry_visitor,
        const RecursiveConstDagVisitor &on_exit_visitor = [](auto /*op*/,
                                                             auto /*dag*/) {});

}  // namespace utils
}  // namespace dag

#endif  // DAG_UTILS_APPLY_HPP
