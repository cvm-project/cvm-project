#ifndef DAG_UTILS_APPLY_VISITOR_HPP
#define DAG_UTILS_APPLY_VISITOR_HPP

#include <functional>

class DAG;
class DAGOperator;

namespace dag {
namespace utils {

using DagVisitor = std::function<void(DAGOperator *)>;

void ApplyInTopologicalOrder(DAG *dag, const DagVisitor &visitor);
void ApplyInTopologicalOrder(const DAG *dag, const DagVisitor &visitor);

void ApplyInReverseTopologicalOrder(DAG *dag, const DagVisitor &visitor);
void ApplyInReverseTopologicalOrder(const DAG *dag, const DagVisitor &visitor);

}  // namespace utils
}  // namespace dag

#endif  // DAG_UTILS_APPLY_HPP
