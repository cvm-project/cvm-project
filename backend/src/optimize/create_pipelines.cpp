#include "create_pipelines.hpp"

#include <string>
#include <vector>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/range/algorithm.hpp>

#include "dag/DAG.h"
#include "dag/DAGOperators.h"
#include "dag/utils/apply_visitor.hpp"
#include "dag/utils/type_traits.hpp"

using dag::utils::IsInstanceOf;

bool IsPipelineDriver(DAGOperator *const op) {
    return IsInstanceOf<              //
            DAGMaterializeRowVector,  //
            DAGEnsureSingleTuple,     //
            DAGGroupBy,               //
            DAGParallelMap            //
            >(op);
}

bool IsSingleTupleProducer(DAGOperator *const op) {
    return IsInstanceOf<              //
            DAGMaterializeRowVector,  //
            DAGEnsureSingleTuple,     //
            DAGReduce                 //
            >(op);
}

bool IsPipelinePredecessor(DAGOperator *const op) {
    return IsInstanceOf<         //
            DAGParameterLookup,  //
            DAGPipeline          //
            >(op);
}

namespace optimize {

void CreatePipelines::Run(DAG *const dag,
                          const std::string & /*config*/) const {
    std::vector<DAGOperator *> pipeline_drivers;

    // Collect pipeline drivers: specific operator types and operators with
    // multiple consumers
    dag::utils::ApplyInReverseTopologicalOrder(dag, [&](DAGOperator *const op) {
        if (dag->out_degree(op) > 1 || IsPipelineDriver(op)) {
            pipeline_drivers.push_back(op);
        }
    });
    assert(boost::algorithm::any_of_equal(pipeline_drivers, dag->output().op));

    // Create a tree-shaped sub-plan for each pipeline driver
    for (const auto driver : pipeline_drivers) {
        // Create pipeline operator
        auto const pop = new DAGPipeline();
        dag->AddOperator(pop);
        dag->set_inner_dag(pop, new DAG());
        auto const inner_dag = dag->inner_dag(pop);
        pop->num_inputs = 0;

        if (dag->output().op == driver) dag->set_output(pop);

        // Connect successors with pipeline
        {
            std::vector<DAG::Flow> out_flows;
            boost::copy(dag->out_flows(driver), std::back_inserter(out_flows));

            for (auto const &f : out_flows) {
                dag->RemoveFlow(f);
                dag->AddFlow(pop, f.source.port, f.target.op, f.target.port);
            }
        }

        // Keep list of in-flows that need to be recreated in the inner DAG
        std::vector<DAG::Flow> pending_in_flows;
        boost::copy(dag->in_flows(driver),
                    std::back_inserter(pending_in_flows));

        for (auto const &f : pending_in_flows) {
            dag->RemoveFlow(f);
        }

        // Move pipeline driver into pipeline
        dag->MoveOperator(inner_dag, driver);
        inner_dag->set_output(driver);

        // Move upstream operators into the pipeline
        while (!pending_in_flows.empty()) {
            auto in_flow = pending_in_flows.back();
            pending_in_flows.pop_back();

            // Handle beginning of pipeline: create input parameter
            if (IsPipelinePredecessor(in_flow.source.op)) {
                // Connect outer predecessor to new input port of the pipeline
                auto const input_port = pop->num_inputs++;
                dag->AddFlow(in_flow.source.op, in_flow.source.port, pop,
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
            boost::copy(dag->in_flows(in_flow.source.op),
                        std::back_inserter(current_in_flows));

            for (auto const &f : current_in_flows) {
                pending_in_flows.push_back(f);
                dag->RemoveFlow(f);
            }

            // Move operator to inner DAG and reconnect to sucessor
            dag->MoveOperator(inner_dag, in_flow.source.op);
            inner_dag->AddFlow(in_flow.source.op, in_flow.source.port,
                               in_flow.target.op, in_flow.target.port);
        }

        // Done with this pipeline; now it should be a tree
        assert(inner_dag->IsTree());

        // If necessary, materialize at end of pipeline and unnest before all
        // consuming operators
        if (!IsSingleTupleProducer(driver)) {
            auto const mat_op = new DAGMaterializeRowVector();
            inner_dag->AddOperator(mat_op);
            inner_dag->AddFlow(inner_dag->output().op, mat_op);
            inner_dag->set_output(mat_op);

            auto const scan_op = new DAGRowScan();
            dag->AddOperator(scan_op);

            std::vector<DAG::Flow> out_flows;
            boost::copy(dag->out_flows(pop), std::back_inserter(out_flows));
            for (auto const f : out_flows) {
                dag->RemoveFlow(f);
                dag->AddFlow(scan_op, f.target.op, f.target.port);
            }

            dag->AddFlow(pop, scan_op);
        }
    }
}

}  // namespace optimize
