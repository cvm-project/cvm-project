#include "dag/utils/type_traits.hpp"

#include "dag/operators/all_operator_definitions.hpp"

namespace dag::utils {

auto IsPipelineDriver(DAGOperator *const op) -> bool {
    return IsInstanceOf<              //
            DAGMaterializeRowVector,  //
            DAGEnsureSingleTuple,     //
            DAGGroupBy,               //
            DAGParallelMap,           //
            DAGParallelMapOmp         //
            >(op);
}

auto IsPipelinePredecessor(DAGOperator *const op) -> bool {
    return IsInstanceOf<         //
            DAGParameterLookup,  //
            DAGPipeline          //
            >(op);
}

auto IsSingleTupleProducer(DAGOperator *const op) -> bool {
    return IsInstanceOf<              //
            DAGMaterializeRowVector,  //
            DAGEnsureSingleTuple,     //
            DAGReduce                 //
            >(op);
}

auto IsSourceOperator(DAGOperator *const op) -> bool {
    return IsInstanceOf<    //
            DAGColumnScan,  //
            DAGRowScan,     //
            DAGRange        //
            >(op);
}

}  // namespace dag::utils
