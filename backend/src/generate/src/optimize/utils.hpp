#ifndef OPTIMIZE_UTILS_HPP
#define OPTIMIZE_UTILS_HPP

class DAGOperator;

namespace optimize {

auto MakeSplitOperator(const DAGOperator *op) -> DAGOperator *;

}  // namespace optimize

#endif  // OPTIMIZE_UTILS_HPP
