#include "dag/utils/type_traits.hpp"

#include "dag/operators/all_operator_definitions.hpp"

namespace dag {
namespace utils {

bool IsPipelineDriver(DAGOperator *const op) {
    return IsInstanceOf<              //
            DAGMaterializeRowVector,  //
            DAGEnsureSingleTuple,     //
            DAGGroupBy,               //
            DAGParallelMap            //
            >(op);
}

bool IsPipelinePredecessor(DAGOperator *const op) {
    return IsInstanceOf<         //
            DAGParameterLookup,  //
            DAGPipeline          //
            >(op);
}

bool IsSingleTupleProducer(DAGOperator *const op) {
    return IsInstanceOf<              //
            DAGMaterializeRowVector,  //
            DAGEnsureSingleTuple,     //
            DAGReduce                 //
            >(op);
}

bool IsSourceOperator(DAGOperator *const op) {
    return IsInstanceOf<    //
            DAGColumnScan,  //
            DAGRowScan,     //
            DAGRange        //
            >(op);
}

}  // namespace utils
}  // namespace dag
