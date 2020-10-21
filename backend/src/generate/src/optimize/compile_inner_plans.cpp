#include "compile_inner_plans.hpp"

#include <utility>

#include <boost/mpl/list.hpp>
#include <nlohmann/json.hpp>

#include "dag/dag.hpp"
#include "dag/operators/concurrent_execute.hpp"
#include "dag/operators/concurrent_execute_lambda.hpp"
#include "dag/operators/concurrent_execute_process.hpp"
#include "dag/operators/operator.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "dag/utils/type_traits.hpp"
#include "optimize/optimizer.hpp"
#include "utils/visitor.hpp"

using dag::utils::IsSingleTupleProducer;

namespace optimize {

class CompileInnerPlanVisitor
    : public Visitor<CompileInnerPlanVisitor, const DAGOperator,
                     boost::mpl::list<                    //
                             DAGConcurrentExecute,        //
                             DAGConcurrentExecuteLambda,  //
                             DAGConcurrentExecuteProcess  //
                             >::type> {
public:
    CompileInnerPlanVisitor(DAG *const dag, std::string config)
        : dag_(dag), config_(std::move(config)) {}

private:
    void HandleOperator(const DAGOperator *const op) {
        assert(IsSingleTupleProducer(dag_->inner_dag(op)->output().op));
        optimize::Optimizer opt(config_);
        opt.Run(dag_->inner_dag(op));
    }

public:
    void operator()(const DAGConcurrentExecute *const op) {
        HandleOperator(op);
    }
    void operator()(const DAGConcurrentExecuteProcess *const op) {
        HandleOperator(op);
    }
    void operator()(const DAGConcurrentExecuteLambda *const op) {
        HandleOperator(op);
    }

private:
    DAG *const dag_;
    const std::string config_;
};

void CompileInnerPlans::Run(DAG *const dag, const std::string &config) const {
    // Compile inner plans
    dag::utils::ApplyInReverseTopologicalOrder(
            dag, [&](const DAGOperator *const op) {
                CompileInnerPlanVisitor visitor(dag, config);
                return visitor.Visit(op);
            });
}

}  // namespace optimize
