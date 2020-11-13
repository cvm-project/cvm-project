#include "two_level_exchange.hpp"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/format.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/irange.hpp>

#include "dag/collection/tuple.hpp"
#include "dag/dag.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/type/array.hpp"
#include "dag/type/atomic.hpp"
#include "dag/type/tuple.hpp"
#include "dag/utils/type_traits.hpp"

using dag::utils::IsInstanceOf;

namespace optimize {

void TwoLevelExchange::Run(DAG *const dag,
                           const std::string & /*config*/) const {
    for (auto *const candidate_pop : dag->operators()) {
        if (!IsInstanceOf<DAGConcurrentExecute>(candidate_pop)) continue;
        auto *const pop = dynamic_cast<DAGConcurrentExecute *>(candidate_pop);
        assert(pop != nullptr);
        auto *const inner_dag = dag->inner_dag(pop);

        std::vector<DAGOperator *> exchange_operators;
        for (auto *const op : inner_dag->operators()) {
            if (IsInstanceOf<DAGExchange>(op)) {
                exchange_operators.push_back(dynamic_cast<DAGExchange *>(op));
            }
        }

        for (auto *const op : exchange_operators) {
            auto *const dag = inner_dag;
            auto const in_flow = dag->in_flow(op);
            auto const out_flow = dag->out_flow(op);

            // Add partitioned exchange operators
            auto *const level0_exchange_op = new DAGPartitionedExchange();
            dag->AddOperator(level0_exchange_op);
            level0_exchange_op->num_levels = 2;
            level0_exchange_op->level_num = 0;

            auto *const level1_exchange_op = new DAGPartitionedExchange();
            dag->AddOperator(level1_exchange_op);
            level1_exchange_op->num_levels = 2;
            level1_exchange_op->level_num = 1;

            // Reconnect DAG
            dag->RemoveFlow(in_flow);
            dag->RemoveFlow(out_flow);
            dag->RemoveOperator(op);

            dag->AddFlow(in_flow.source, level0_exchange_op);
            dag->AddFlow(level0_exchange_op, level1_exchange_op);
            dag->AddFlow(level1_exchange_op, out_flow.target);
        }
    }
}

}  // namespace optimize
