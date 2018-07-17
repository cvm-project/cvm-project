#include "create_pipelines.hpp"

#include <unordered_set>

#include <boost/mpl/list.hpp>
#include <boost/range/algorithm.hpp>

#include "dag/DAGOperators.h"
#include "utils/visitor.hpp"

class CollectPipelineDriversVisitor
    : public Visitor<CollectPipelineDriversVisitor, DAGOperator,
                     boost::mpl::list<                 //
                             DAGMaterializeRowVector,  //
                             DAGEnsureSingleTuple      //
                             >::type> {
public:
    void operator()(DAGMaterializeRowVector *const op) {
        pipeline_drivers_.insert(op);
    }
    void operator()(DAGEnsureSingleTuple *const op) {
        pipeline_drivers_.insert(op);
    }

    std::unordered_set<DAGOperator *> pipeline_drivers_;
};

void CreatePipelines::optimize() {
    std::unordered_set<DAGOperator *> pipeline_drivers;

    // Collect pipeline drivers: multiple consumers
    for (auto const op : dag_->operators()) {
        if (dag_->out_degree(op) > 1) pipeline_drivers.insert(op);
    }

    // Collect pipeline drivers: main sink
    pipeline_drivers.insert(dag_->output().op);

    // Collect pipeline drivers: materialization points
    CollectPipelineDriversVisitor collect_visitor;
    for (auto const op : dag_->operators()) {
        collect_visitor.Visit(op);
    }
    std::copy(collect_visitor.pipeline_drivers_.begin(),
              collect_visitor.pipeline_drivers_.end(),
              std::inserter(pipeline_drivers, pipeline_drivers.end()));

    // Create a tree-shaped sub-plan for each pipeline driver
    for (const auto driver : pipeline_drivers) {
        // Create pipeline operator
        auto const pop = new DAGPipeline();
        dag_->AddOperator(pop);
        dag_->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag_->inner_dag(pop);
        pop->num_inputs = 0;

        if (dag_->output().op == driver) dag_->set_output(pop);

        // Connect successors with pipeline
        {
            std::vector<DAG::Flow> out_flows;
            boost::copy(dag_->out_flows(driver), std::back_inserter(out_flows));

            for (auto const &f : out_flows) {
                dag_->RemoveFlow(f);
                dag_->AddFlow(pop, f.source.port, f.target.op, f.target.port);
            }
        }

        // Keep list of in-flows that need to be recreated in the inner DAG
        std::vector<DAG::Flow> pending_in_flows;
        boost::copy(dag_->in_flows(driver),
                    std::back_inserter(pending_in_flows));

        for (auto const &f : pending_in_flows) {
            dag_->RemoveFlow(f);
        }

        // Move pipeline driver into pipeline
        dag_->MoveOperator(inner_dag, driver);
        inner_dag->set_output(driver);

        // Move upstream operators into the pipeline
        while (!pending_in_flows.empty()) {
            auto in_flow = pending_in_flows.back();
            pending_in_flows.pop_back();

            // Handle beginning of pipeline: create input parameter
            if (pipeline_drivers.count(in_flow.source.op) > 0 ||
                dag_->in_degree(in_flow.source.op) == 0 ||
                in_flow.source.op->name() == "pipeline") {
                // Connect outer predecessor to new input port of the pipeline
                auto const input_port = pop->num_inputs++;
                dag_->AddFlow(in_flow.source.op, in_flow.source.port, pop,
                              input_port);

                // Connect inner successor to parameter lookup of that port
                const auto param_op = new DAGParameterLookup();
                inner_dag->AddOperator(param_op);
                inner_dag->set_input(input_port, param_op);
                inner_dag->AddFlow(param_op, in_flow.target.op,
                                   in_flow.target.port);
                continue;
            }

            // Remove in-flows from outer DAG
            std::vector<DAG::Flow> current_in_flows;
            boost::copy(dag_->in_flows(in_flow.source.op),
                        std::back_inserter(current_in_flows));

            for (auto const &f : current_in_flows) {
                pending_in_flows.push_back(f);
                dag_->RemoveFlow(f);
            }

            // Move operator to inner DAG and reconnect to sucessor
            dag_->MoveOperator(inner_dag, in_flow.source.op);
            inner_dag->AddFlow(in_flow.source.op, in_flow.source.port,
                               in_flow.target.op, in_flow.target.port);
        }
    }
}